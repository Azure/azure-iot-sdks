/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package samples.com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.sdk.JobProperties;
import com.microsoft.azure.iot.service.sdk.RegistryManager;
import com.microsoft.azure.storage.CloudStorageAccount;
import com.microsoft.azure.storage.blob.CloudBlob;
import com.microsoft.azure.storage.blob.CloudBlobClient;
import com.microsoft.azure.storage.blob.CloudBlobContainer;
import com.microsoft.azure.storage.blob.ListBlobItem;

import java.io.FileOutputStream;

public class DeviceManagerExportSample
{
    private static final Boolean excludeKeys = false;
    public static final String sampleContainerName = "exportsample";

    public static void main(String[] args) throws Exception
    {
        System.out.println("Starting export sample...");

        CloudStorageAccount storageAccount = CloudStorageAccount.parse(SampleUtils.storageConnectionString);
        CloudBlobClient blobClient = storageAccount.createCloudBlobClient();
        CloudBlobContainer container = blobClient.getContainerReference(DeviceManagerExportSample.sampleContainerName);
        container.createIfNotExists();
        String containerSasUri = SampleUtils.getContainerSasUri(container);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(SampleUtils.iotHubConnectionString);
        JobProperties exportJob = registryManager.exportDevices(containerSasUri, excludeKeys);

        while(true)
        {
            exportJob = registryManager.getJob(exportJob.getJobId());
            if (exportJob.getStatus() == JobProperties.JobStatus.COMPLETED)
            {
                break;
            }
            Thread.sleep(500);
        }

        for(ListBlobItem blobItem : container.listBlobs())
        {
            if (blobItem instanceof CloudBlob)
            {
                CloudBlob blob = (CloudBlob) blobItem;
                blob.download(new FileOutputStream(SampleUtils.exportFileLocation + blob.getName()));
            }
        }

        System.out.println("Export job completed. Results are in " + SampleUtils.exportFileLocation);
    }
}