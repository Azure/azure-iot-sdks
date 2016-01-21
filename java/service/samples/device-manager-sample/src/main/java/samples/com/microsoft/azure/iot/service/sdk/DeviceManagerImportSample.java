/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package samples.com.microsoft.azure.iot.service.sdk;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.microsoft.azure.iot.service.sdk.*;
import com.microsoft.azure.storage.CloudStorageAccount;
import com.microsoft.azure.storage.blob.*;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.UUID;

public class DeviceManagerImportSample
{
    private static String importContainerName = "importcontainersample";
    private static String importBlobName = "devices.txt";
    private static Gson gson = new GsonBuilder().disableHtmlEscaping().create();
    private static Integer numberOfDevice = 10;

    public static void main(String[] args) throws Exception
    {
        System.out.println("Starting import sample...");

        // Creating Azure storage container and getting its URI
        CloudStorageAccount storageAccount = CloudStorageAccount.parse(SampleUtils.storageConnectionString);
        CloudBlobClient blobClient = storageAccount.createCloudBlobClient();
        CloudBlobContainer container = blobClient.getContainerReference(DeviceManagerImportSample.importContainerName);
        container.createIfNotExists();
        String containerSasUri = SampleUtils.getContainerSasUri(container);

        // Creating the list of devices to be submitted for import
        StringBuilder devicesToImport = new StringBuilder();
        for (int i = 0; i < 1; i++)
        {
            String deviceId = UUID.randomUUID().toString();
            Device device = Device.createFromId(deviceId, null, null);
            AuthenticationMechanism authentication = new AuthenticationMechanism(device.getSymmetricKey());

            ExportImportDevice deviceToAdd = new ExportImportDevice();
            deviceToAdd.setId(deviceId);
            deviceToAdd.setAuthentication(authentication);
            deviceToAdd.setStatus(DeviceStatus.Enabled);
            deviceToAdd.setImportMode(ImportMode.CreateOrUpdate);

            devicesToImport.append(gson.toJson(deviceToAdd));

            if (i < numberOfDevice - 1)
            {
                devicesToImport.append("\r\n");
            }
        }

        byte[] blobToImport = devicesToImport.toString().getBytes(StandardCharsets.UTF_8);

        // Creating the Azure storage blob and uploading the serialized string of devices
        System.out.println("Uploading " + blobToImport.length + " bytes into Azure storage.");
        InputStream stream = new ByteArrayInputStream(blobToImport);
        CloudBlockBlob importBlob = container.getBlockBlobReference(DeviceManagerImportSample.importBlobName);
        importBlob.deleteIfExists();
        importBlob.upload(stream, blobToImport.length);

        // Starting the import job
        RegistryManager registryManager = RegistryManager.createFromConnectionString(SampleUtils.iotHubConnectionString);
        JobProperties importJob = registryManager.importDevices(containerSasUri, containerSasUri);

        // Waiting for the import job to complete
        while(true)
        {
            importJob = registryManager.getJob(importJob.getJobId());
            if (importJob.getStatus() == JobProperties.JobStatus.COMPLETED
                    || importJob.getStatus() == JobProperties.JobStatus.FAILED)
            {
                break;
            }
            Thread.sleep(500);
        }

        // Checking the result of the import job
        if (importJob.getStatus() == JobProperties.JobStatus.COMPLETED)
        {
            System.out.println("Import job completed. The new devices are now added to the hub.");
        }
        else
        {
            System.out.println("Import job failed. Failure reason: " + importJob.getFailureReason());
        }

        //Cleaning up the blob
        for(ListBlobItem blobItem : container.listBlobs())
        {
            if (blobItem instanceof CloudBlob)
            {
                CloudBlob blob = (CloudBlockBlob) blobItem;
                blob.deleteIfExists();
            }
        }
    }
}