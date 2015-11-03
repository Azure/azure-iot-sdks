msbuild UWPTests.sln /p:Publish=true /p:Platform=x86

if ($?) {

    VSTest.Console.exe AppPackages\UWPTests_1.0.0.0_x86_Debug_Test\UWPTests_1.0.0.0_x86_Debug.appx /InIsolation 

    if($?) {
        Write-Host "Test execution succeeded!" -foregroundcolor green
        return 0
    }
    Write-Host "Test execution failed!" -foregroundcolor red
}

return 1