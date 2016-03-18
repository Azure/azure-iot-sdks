/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.integration.com.microsoft.azure.iot.service.sdk;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.sdk.*;
import com.microsoft.azure.storage.CloudStorageAccount;
import com.microsoft.azure.storage.StorageException;
import com.microsoft.azure.storage.blob.*;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URISyntaxException;
import java.nio.charset.StandardCharsets;
import java.security.InvalidKeyException;
import java.time.Duration;
import java.time.Instant;
import java.util.*;

import static org.junit.Assert.assertEquals;

public class RegistryManagerIT
{
    private static String iotHubonnectionStringEnvVarName = "IOTHUB_CONNECTION_STRING";
    private static String storageAccountConnectionStringEnvVarName = "STORAGE_ACCOUNT_CONNECTION_STRING";
    private static String iotHubConnectionString = "";
    private static String storageAccountConnextionString = "";
    private static String deviceId = "java-crud-e2e-test";

    private static Gson gson = new GsonBuilder().disableHtmlEscaping().create();
    private static CloudBlobClient blobClient;

    @Before
    public void setUp() throws URISyntaxException, InvalidKeyException
    {
        Map<String, String> env = System.getenv();
        for (String envName : env.keySet())
        {
            if (envName.equals(iotHubonnectionStringEnvVarName.toString()))
            {
                iotHubConnectionString = env.get(envName);
            }
            else if (envName.equals(storageAccountConnectionStringEnvVarName.toString()))
            {
                storageAccountConnextionString = env.get(envName);
            }
        }

        String uuid = UUID.randomUUID().toString();
        deviceId = deviceId.concat("-" + uuid);

        CloudStorageAccount storageAccount = CloudStorageAccount.parse(storageAccountConnextionString);
        blobClient = storageAccount.createCloudBlobClient();
    }

    @Test
    public void crud_e2e() throws Exception
    {
        Boolean deviceExist = false;
        if (Tools.isNullOrEmpty(iotHubConnectionString))
        {
            throw new IllegalArgumentException("Environment variable is not set: " + iotHubonnectionStringEnvVarName);
        }

        // Arrange
        // Check if device exists with the given name
        RegistryManager registryManager = RegistryManager.createFromConnectionString(iotHubConnectionString);
        try
        {
            Device device = registryManager.getDevice(deviceId);
            deviceExist = true;
        } catch (IotHubException e)
        {
        }
        if (deviceExist)
        {
            try
            {
                registryManager.removeDevice(deviceId);
            } catch (IotHubException|IOException e)
            {
                System.out.println("Initialization failed, could not remove device: " + deviceId);
                e.printStackTrace();
            }
        }
        // Act

        // Create
        Device deviceAdded = Device.createFromId(deviceId, null, null);
        registryManager.addDevice(deviceAdded);

        // Read
        Device deviceGet = registryManager.getDevice(deviceId);

        // Delete
        registryManager.removeDevice(deviceId);

        // Assert
        assertEquals(deviceId, deviceAdded.getDeviceId());
        assertEquals(deviceId, deviceGet.getDeviceId());
        try
        {
            Device device = registryManager.getDevice(deviceId);
        } catch (IotHubException e)
        {
        }
    }

    @Test
    public void getDeviceStatisticsTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(iotHubConnectionString))
        {
            // Arrange
            RegistryManager registryManager = RegistryManager.createFromConnectionString(iotHubConnectionString);
            // Act
            registryManager.getStatistics();
        }
    }

    @Test
    public void export_import_e2e() throws Exception
    {
        Boolean excludeKeys = false;
        String exportContainerName = "exportcontainersample";
        String importContainerName = "importcontainersample";
        Integer numberOfDevices = 10;

        // Creating the export storage container and getting its URI
        CloudBlobContainer exportContainer = blobClient.getContainerReference(exportContainerName);
        exportContainer.createIfNotExists();
        String containerSasUri = getContainerSasUri(exportContainer);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(iotHubConnectionString);
        JobProperties exportJob = registryManager.exportDevices(containerSasUri, excludeKeys);

        JobProperties.JobStatus jobStatus;

        while(true)
        {
            exportJob = registryManager.getJob(exportJob.getJobId());
            jobStatus = exportJob.getStatus();
            if (jobStatus == JobProperties.JobStatus.COMPLETED || jobStatus == JobProperties.JobStatus.FAILED)
            {
                break;
            }
            Thread.sleep(100);
        }

        exportContainer.deleteIfExists();

        if (jobStatus != JobProperties.JobStatus.COMPLETED)
        {
            Assert.fail("The export job was not completed successfully");
        }

        // Creating the import storage container and getting its URI
        CloudBlobContainer importContainer = blobClient.getContainerReference(importContainerName);
        importContainer.createIfNotExists();

        //Creating the list of devices to be created, then deleted
        List<ExportImportDevice> devices = new ArrayList<>(numberOfDevices);
        for (int i = 0; i < numberOfDevices; i++)
        {
            String deviceId = UUID.randomUUID().toString();
            Device device = Device.createFromId(deviceId, null, null);
            AuthenticationMechanism authentication = new AuthenticationMechanism(device.getSymmetricKey());

            ExportImportDevice deviceToAdd = new ExportImportDevice();
            deviceToAdd.setId(deviceId);
            deviceToAdd.setAuthentication(authentication);
            deviceToAdd.setStatus(DeviceStatus.Enabled);

            devices.add(deviceToAdd);
        }

        //importing devices - create mode
        runImportJob(importContainer, devices, ImportMode.CreateOrUpdate);

        //importing devices - delete mode
        runImportJob(importContainer, devices, ImportMode.Delete);

        //Cleaning up the container
        importContainer.deleteIfExists();
    }

    private static String getContainerSasUri(CloudBlobContainer container) throws InvalidKeyException, StorageException
    {
        //Set the expiry time and permissions for the container.
        //In this case no start time is specified, so the shared access signature becomes valid immediately.
        SharedAccessBlobPolicy sasConstraints = new SharedAccessBlobPolicy();
        Date expirationDate = Date.from(Instant.now().plus(Duration.ofDays(1)));
        sasConstraints.setSharedAccessExpiryTime(expirationDate);
        EnumSet<SharedAccessBlobPermissions> permissions = EnumSet.of(
                SharedAccessBlobPermissions.WRITE,
                SharedAccessBlobPermissions.LIST,
                SharedAccessBlobPermissions.READ,
                SharedAccessBlobPermissions.DELETE);
        sasConstraints.setPermissions(permissions);

        //Generate the shared access signature on the container, setting the constraints directly on the signature.
        String sasContainerToken = container.generateSharedAccessSignature(sasConstraints, null);

        //Return the URI string for the container, including the SAS token.
        return container.getUri() + "?" + sasContainerToken;
    }

    private void runImportJob(CloudBlobContainer container, List<ExportImportDevice> devices, ImportMode importMode) throws Exception
    {
        // Creating the json string to be submitted for import using the specified importMode
        StringBuilder devicesToAdd = new StringBuilder();
        for (int i = 0; i < devices.size(); i++)
        {
            devices.get(i).setImportMode(importMode);
            devicesToAdd.append(gson.toJson(devices.get(i)));

            if (i < devices.size() - 1)
            {
                devicesToAdd.append("\r\n");
            }
        }

        byte[] blobToImport = devicesToAdd.toString().getBytes(StandardCharsets.UTF_8);

        // Creating the Azure storage blob and uploading the serialized string of devices
        InputStream stream = new ByteArrayInputStream(blobToImport);
        String importBlobName = "devices.txt";
        CloudBlockBlob importBlob = container.getBlockBlobReference(importBlobName);
        importBlob.deleteIfExists();
        importBlob.upload(stream, blobToImport.length);

        // Starting the import job
        RegistryManager registryManager = RegistryManager.createFromConnectionString(iotHubConnectionString);
        JobProperties importJob = registryManager.importDevices(getContainerSasUri(container), getContainerSasUri(container));

        // Waiting for the import job to complete
        while(true)
        {
            importJob = registryManager.getJob(importJob.getJobId());
            if (importJob.getStatus() == JobProperties.JobStatus.COMPLETED
                    || importJob.getStatus() == JobProperties.JobStatus.FAILED)
            {
                break;
            }
            Thread.sleep(100);
        }

        // Checking the result of the import job
        if (importJob.getStatus() != JobProperties.JobStatus.COMPLETED)
        {
            Assert.fail("The import job was not completed successfully for " + importMode + " operation.");
        }
    }
}