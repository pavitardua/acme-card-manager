# LightWave ACME Card Manager
ACME Card Manager simulates how a traditional NonStop payment application (such as BASE24 or Connex) might be REST-enabled using LightWave Server. Typical account and card functions are supported from the relevant menu items. Transactions can be initiated from the Terminal view with the results viewed on the Transactions view. A small range of relevant response codes are simulated, and the result of the transaction (success or failure) can be seen in real-time in the Terminal view, and historically in the Transactions view.

All of these services are provided as REST APIs by LightWave Server working in conjunction with simple demo Pathway Servers on the NonStop. The details of all data exchanged with the REST services can be viewed in the API Request Log view - this shows all requests sent to the NonStop, and all responses returned.

When enabled, the application Pathway servers use LightWave Client to send SMS notifications to simulated cardholders using the [AWS Simple Notification Service](https://aws.amazon.com/sns) REST API. Notifications are sent for sale and void transactions, and card lock status changes.

![](./acme_card_diagram.png =900x)

The sample consists of the Pathway application and a browser based client application. These compoents may be installed using these instructions:

| Installation Instructions |
| -- | 
| [NonStop Pathway Application](service) |
| [Browser based client application](web-app) |


Assistance is available through the [NuWave Technologies Support Center](http://support.nuwavetech.com).

