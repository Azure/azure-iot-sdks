/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.google.gson.annotations.SerializedName;

import java.util.Date;

public class JobProperties {
    public JobProperties()
    {
        this.setJobId("");
    }

    /**
     * @return the system generated job id. Ignored at creation.
     */
    public String getJobId() {
        return JobId;
    }

    /**
     * @param jobId the job id
     */
    public void setJobId(String jobId) {
        JobId = jobId;
    }

    /**
     * @return the system generated UTC job start time. Ignored at creation.
     */
    public Date getStartTimeUtc() {
        return StartTimeUtc;
    }

    /**
     * @param startTimeUtc the UTC job start time.
     */
    public void setStartTimeUtc(Date startTimeUtc) {
        StartTimeUtc = startTimeUtc;
    }

    /**
     * @return the UTC job end time. Ignored at creation.
     * Represents the time the job stopped processing.
     */
    public Date getEndTimeUtc() {
        return EndTimeUtc;
    }

    /**
     * @param endTimeUtc the UTC job end time.
     */
    public void setEndTimeUtc(Date endTimeUtc) {
        EndTimeUtc = endTimeUtc;
    }

    /**
     * @return the type of job to execute.
     */
    public JobType getType() {
        return Type;
    }

    /**
     * @param type the type of job to execute.
     */
    public void setType(JobType type) {
        Type = type;
    }

    /**
     * @return the system generated job status. Ignored at creation.
     */
    public JobStatus getStatus() {
        return Status;
    }

    /**
     * @param status the job status.
     */
    public void setStatus(JobStatus status) {
        Status = status;
    }

    /**
     * @return the system generated job progress. Ignored at creation.
     * Represents the completion percentage.
     */
    public int getProgress() {
        return Progress;
    }

    /**
     * @param progress the job progress.
     */
    public void setProgress(int progress) {
        Progress = progress;
    }

    /**
     * @return URI containing SAS token to a blob container that contains registry data to sync.
     */
    public String getInputBlobContainerUri() {
        return InputBlobContainerUri;
    }

    /**
     * @param inputBlobContainerUri the input blob container URI.
     */
    public void setInputBlobContainerUri(String inputBlobContainerUri) {
        InputBlobContainerUri = inputBlobContainerUri;
    }

    /**
     * @return URI containing SAS token to a blob container.
     * This is used to output the status of the job and the results.
     */
    public String getOutputBlobContainerUri() {
        return OutputBlobContainerUri;
    }

    /**
     * @param outputBlobContainerUri the output blob container URI.
     */
    public void setOutputBlobContainerUri(String outputBlobContainerUri) {
        OutputBlobContainerUri = outputBlobContainerUri;
    }

    /**
     * @return whether the keys are included in export or not.
     */
    public boolean getExcludeKeysInExport() {
        return ExcludeKeysInExport;
    }

    /**
     * @param excludeKeysInExport optional for export jobs; ignored for other jobs.  Default: false.
     * If false, authorization keys are included in export output.  Keys are exported as null otherwise.
     */
    public void setExcludeKeysInExport(boolean excludeKeysInExport) {
        ExcludeKeysInExport = excludeKeysInExport;
    }

    /**
     * @return System generated. Ignored at creation.
     * If status == failure, this represents a string containing the reason.
     */
    public String getFailureReason() {
        return FailureReason;
    }

    /**
     * @param failureReason the failure reason.
     */
    public void setFailureReason(String failureReason) {
        FailureReason = failureReason;
    }

    public enum JobType
    {
        @SerializedName("unknown")
        UNKNOWN,

        @SerializedName("export")
        EXPORT,

        @SerializedName("import")
        IMPORT
    }

    public enum JobStatus
    {
        @SerializedName("unknown")
        UNKNOWN,

        @SerializedName("enqueued")
        ENQUEUED,

        @SerializedName("running")
        RUNNING,

        @SerializedName("completed")
        COMPLETED,

        @SerializedName("failed")
        FAILED,

        @SerializedName("cancelled")
        CANCELLED
    }

    // CODES_SRS_SERVICE_SDK_JAVA_JOB_PROPERTIES_15_001: [The JobProperties class has the following properties: JobId,
    // StartTimeUtc, EndTimeUtc, JobType, JobStatus, Progress, InputBlobContainerUri, OutputBlobContainerUri,
    // ExcludeKeysInExport, FailureReason.]

    @SerializedName("jobId")
    private String JobId;

    @SerializedName("startTimeUtc")
    private Date StartTimeUtc;

    @SerializedName("endTimeUtc")
    private Date EndTimeUtc;

    @SerializedName("type")
    private JobType Type;

    @SerializedName("status")
    private JobStatus Status;

    @SerializedName("progress")
    private int Progress;

    @SerializedName("inputBlobContainerUri")
    private String InputBlobContainerUri;

    @SerializedName("outputBlobContainerUri")
    private String OutputBlobContainerUri;

    @SerializedName("excludeKeysInExport")
    private boolean ExcludeKeysInExport;

    @SerializedName("failureReason")
    private String FailureReason;
}