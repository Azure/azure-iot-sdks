set -e

IOTParametersFilePath=$1

if test -z $IOTParametersFilePath
then
	IOTParametersFilePath="./iot_device_params.txt"
fi

echo "Reading parameters from: $IOTParametersFilePath"

awk 'BEGIN {
	TargetFile="/etc/environment"

	EnvVarsLength = 0;

	while ((getline EnvStr < TargetFile) != 0)
	{
		EnvVarsLength++;
		EnvVars[EnvVarsLength] = EnvStr;
	}
} {
	if (length($0) > 0 && substr($0, 1, 1) != "#")
	{
		print "Writing " $0;

		if ((i = index($0, "=")) != 0)
		{
			# This gets the string including '='.
			Name = substr($0, 1, i);
			# Value = substr($0, i + 1, length($0) - i + 1);

			EnvVarIsPresent = 0;
			NewIndex = 1;
			for (i in EnvVars)
			{
				if (index(EnvVars[i], Name) != 0)
				{
					EnvVars[i] = $0;
					EnvVarIsPresent = 1;
				}
				
				NewIndex++;
			}

			if (EnvVarIsPresent == 0)
			{
				EnvVars[NewIndex] = $0;
				EnvVarsLength = NewIndex;
			}
			else
			{
				EnvVarsLength = NewIndex - 1;
			}
		}
	}
} END {
        system("mv " TargetFile " " TargetFile ".bkp")

        for (i = 1; i <= EnvVarsLength; i++)
        {
			print EnvVars[i] >> TargetFile;
        }
		
		print "Writing Environment Variable Complete"
}' $IOTParametersFilePath

