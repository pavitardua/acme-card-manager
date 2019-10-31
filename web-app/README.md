# LightWave ACME Card Manager Web Application
The ACME Card Manager Web Application is a Single Page Application (SPA) that implements the User Interface for the ACME Card Service. The entire application is implemented in a handful of HTML, Javascript, CSS, and image files. The files can be deployed to any HTTP server capable of hosting static content.

The application must be accessed from an HTTP server. <em>It will not work by simply opening the files in a browser from the desktop filesystem.</em> There are a number of solutions available that will allow you to deploy the application to a web server, for example:
+ Deploy to a local copy of [Apache](https://httpd.apache.org/) or [Nginx](https://www.nginx.com/) server.
+ Deploy to an AWS S3 bucket configured for [static web site hosting](https://docs.aws.amazon.com/AmazonS3/latest/dev/WebsiteHosting.html).
+ If using Visual Studio Code, use the [Live Server extension](https://marketplace.visualstudio.com/items?itemName=ritwickdey.LiveServer).
+ If using Node.js, use the [http-server package](https://www.npmjs.com/package/http-server).

Any HTTP server capable of hosting static pages is sufficient. All of the files in the [app](app) folder should be deployed to the HTTP server. 

### Configuration
The LightWave Server service port must be configured. Edit the settings.json file to configure the host name and service port of your LightWave Server instance.

```javascript
{
  "baseUrl": "http://your-host:your-service-port"
} 
```
Note that the port specified should be the service port on which the ACME Service is deployed, not the the Server Console port. After configuring the port, you may need to refresh your browser in order for the change to take effect.

### Mobile phone number format for SMS notification
If the ACME Card Service is configured to send SMS notifications, the phone number associated with a user account must be formatted correctly. The number must be prefixed with "+" and followed by the country code and number, with no embedded spaces. Hyphens in the number are allowed if appropriate. SMS notification is skipped for any number that is not prefixed with "+". For example:
+ 603-555-1234 - "+" omitted - notification is skipped.
+ +603-555-1234 - Invalid - has US area code but no country code
+ +16035551234 - Correct format for US mobile number
+ +61419555123 - Correct format for Australia mobile number

