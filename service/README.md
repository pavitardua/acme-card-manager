# LightWave ACME Card Manager Service

## Prerequisites

+ NonStop C Compiler.
+ An installed instance of [LightWave Server](https://docs.nuwavetech.com/display/LWSERVER) version 1.0.8 or greater.
 
##### To enable SMS Notifications
[AWS Simple Notification Service](https://docs.aws.amazon.com/sns) is used to send SMS messages. Some familiarity with AWS services may be required in order to successfully enable SMS. SMS notification requires:
+ An installed instance of [LightWave Client](https://docs.nuwavetech.com/display/LWCLIENT) version 1.1.0 or greater.
+ An AWS account.  
+ The security credentials (Access key ID and Secret access key) of an IAM user that has permission to send text messages via SNS (sns:Publish)

## Configuration
The following values must be configured in the SETUP TACL macro after installation of the sample: 

+ pathmon-name - The process name of the Pathmon. The default is $ACME.
+ lws-isv - The installation subvolume of an installed and running LightWave Server instance. The default is the current subvolume.
+ enable-sms - Set to 1 to enable SMS. The default is 0, or disabled.
+ lwc-isv - The installation subvolume of an installed LightWave Client instance. Required only if SMS notification is enabled. Note that the CONSOLE process does not need to be running. Required only if SMS notification is enabled. The default is the current subvolume.
+ sns-base-url - The region specific base url for the SNS endpoint. Required only if SMS notification is enabled.

If using SMS, the AWSAUTH file must be updated with the credentials of an IAM user that has permission to access SNS. For more information on configuring AWS credentials for use with LightWave Client, see [Request Authentication and Signing](https://docs.nuwavetech.com/display/LWCLIENT/Request+Authentication+and+Signing) in the LightWave Client documentation.

The LightWave Server API is configured to use the value of =ACME-PATHMON DEFINE as the Pathmon process name. This DEFINE is set by the SETENV macro. The LightWave Server must be restarted with the DEFINE set in order to for the DEFINE to recognized.

## Installation

The application is built from source. The source files are present in the repository for convenient viewing. 
In addition, a PAK archive containing all of the source files is available for transfer to your NonStop system.

| Repository File | NonStop File |
| -- | -- |
| macros/build.txt | build |
| macros/setenv.txt | setenv |
| macros/startpw.txt | startpw |
| macros/stoppw.txt | stoppw | 
| macros/unsetenv.txt | unsetenv |
| resources/acmeapi.txt | acmeapi |
| resources/acmedict.txt | acmedict |
| resources/acmeddl.txt | acmeddl |
| resources/acmesvc.txt | acmesvc |
| resources/datapak.bin | datapak |
| resources/awsauth.txt | awsauth |
| resources/logcfg.txt | logcfg |
| resources/snsapi.json | snsapi |
| resources/snsddl.txt | snsddl |
| src/acct.c | acctc |
| src/acme.h | acmeh |
| src/card.c | cardc |
| src/pay.c | payc |
| src/sns.h | snsh |
| acmepak.bin | acmepak |  

#### Transfer the PAK archive to your NonStop System

Download `acmepak.bin` from this repository to your local system, then upload to your NonStop server using binary transfer mode.

Note: to download the PAK file, click `acmepak.bin` in the file list to display the file details, then click the *Download* button.

Logon to TACL on your NonStop system to peform the installation and build steps.

#### Unpak the PAK archive
```
TACL > UNPAK STSPAK ($*.*.*), VOL $your-volume.your-subvolume.*, LISTALL, MYID
```
#### Customize and run SETENV
After running SETENV, restart LightWave Server so that it will recognize the =ACME-PATHMON define.
```
TACL> T/EDIT SETENV
TACL> RUN SETENV
```
#### If using SMS, customize the AWSAUTH file.
```
TACL> T/EDIT AWSAUTH
```
#### Build the application DDL dictionary and servers
```
TACL > RUN BUILD
```
#### Install the LightWave Server service detinitions
```
TACL > RUN INSTSVC
```
#### Optionally install the sample data
After installation, the application files are empty. This process will install sample data with one sample user and card.
```
TACL > RUN UNPAKDAT
```
#### Start the Pathway
```
TACL > RUN STARTPW
```
