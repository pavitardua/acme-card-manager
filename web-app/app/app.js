"use strict";

$(async function () {

  /* Setup state. */
  let state = {
    requestLog: [],
    requestLogMaxEntries: 25
  }

  state.settings = await $.getJSON("./settings.json");

  let responseCodeStrings = {
    "00": "Approved",
    "14": "Invalid Card Number",
    "51": "Insufficient Funds",
    "54": "Card Expired",
    "80": "Invalid Date",
    "82": "Invalid CVV",
    "98": "Card Restricted"
  }

  $(document).ready(async function () {
    /* Initialize feather icons. */
    feather.replace();

    /* Initialize toasts. */
    $(".toast").toast({ delay: 2000 });

    /* Register click handlers. */
    $("#clear-request-log").on("click", () => {
      state.requestLog = [];
      state.requestLogIndex = 0;
      viewRequestLog();
    });

    $(".back-button").on("click", () => {
      window.history.back();
    });

    $("#new-account-button").on("click", () => {
      go("account");
    });

    $("#new-card-button").on("click", () => {
      go("card");
    });

    $("#clear-payment-button").on("click", () => {
      clearPaymentForm();
    });

    $("#account-view form").on("submit", (e) => {
      e.preventDefault();
      saveAccount();
    });

    $("#settings-view form").on("submit", (e) => {
      e.preventDefault();
      saveSettings();
    });

    $("#card-view form").on("submit", (e) => {
      e.preventDefault();
      saveCard();
    });

    $("#payment-view form").on("submit", (e) => {
      e.preventDefault();
      submitPayment();
    });

    $("#delete-account-confirmed").on("click", () => {
      deleteAccount();
      $("#delete-account-modal").modal("hide");
    });

    $("#delete-card-confirmed").on("click", () => {
      deleteCard();
      $("#delete-card-modal").modal("hide");
    });

    $("#void-payment-confirmed").on("click", () => {
      voidPayment();
      $("#void-payment-modal").modal("hide");
    });

    /*
    * Configure jQuery Typehead to autocomplete the payment form
    * card number. A payments terminal would not typically do this
    * but it adds convenience to the demo and illustrates one way to 
    * do autocomplete of input fields. For more info in jQuery Typeahead
    * see: http://www.runningcoder.org/jquerytypeahead/
    */
    $("#payment-view #cardNumber").autocomplete({
    });

    $("#payment-view #cardNumber").on("autocompleteselect", function (event, ui) {
      onPaymentCardSelected(ui);
    });

    /* Setup the hash change listener go to the default view. */
    $(window).on("hashchange", () => {
      let path = decodeURI(window.location.hash).substring(1);
      onHashChange(path);
    });

    go();
  });

  function clearPaymentForm() {
    $("#payment-form").find("input[type=text], select").val("");
  }

  async function deleteAccount() {
    let accountNumber = $("#account-view #accountNumber").val();

    let response = await doApiRequest({ method: "DELETE", uri: "/acme/account/" + accountNumber });

    if (response.status !== 204) {
      raiseAlert("error", response.json.errorMessage);
      go("accounts");
      return;
    }

    raiseAlert("success", "Account " + accountNumber + " has been deleted.");
    go("accounts");
  }

  async function deleteCard() {
    let cardNumber = $("#card-view #cardNumber").val();

    let response = await doApiRequest({ method: "DELETE", uri: "/acme/card/" + cardNumber });

    if (response.status !== 204) {
      raiseAlert("error", response.json.errorMessage);
      go("cards");
      return;
    }

    raiseAlert("success", "Card " + cardNumber + " has been deleted.");
    go("cards");
  }

  async function doApiRequest(request) {
    let settings = {};
    const eol = "\n";

    settings.method = request.method;
    settings.url = state.settings.baseUrl + request.uri;
    if (typeof request.json !== "undefined") {
      settings.data = JSON.stringify(request.json, null, 2);
      settings.contentType = "application/json";
    } else if (typeof request.text !== "undefined") {
      settings.data = request.text;
      settings.contentType = "text/plain";
    }

    /* Create the request portion of the request log entry. */
    let rq = request.method + " " + request.uri + " " + "HTTP/1.1" + eol;
    if (typeof settings.data !== "undefined") {
      rq += "content-type: " + settings.contentType + eol;
      rq += "content-length: " + settings.data.length + eol + eol;
      rq += (typeof request.json !== "undefined" ? JSON.stringify(request.json, null, 2) : request.text) + eol;
    } else {
      rq += "Content-Length: 0" + eol;
    }

    let response = await new Promise((resolve, reject) => {
      $.ajax(settings).done((data, status, xhr) => {
        resolve({ status: xhr.status, headers: xhr.getAllResponseHeaders(), text: xhr.responseText, json: xhr.responseJSON });
      }).fail((xhr, status, error) => {
        resolve({ status: xhr.status, headers: xhr.getAllResponseHeaders(), text: xhr.responseText, json: xhr.responseJSON });
      }).always((data, status, xhr) => {

        if (status === "error") {
          xhr = data;
        }

        let rs = "HTTP/1.1 " + xhr.status + " " + xhr.statusText + eol;
        rs += xhr.getAllResponseHeaders();
        rs += eol;

        if (typeof data !== "undefined") {
          try {
            rs += JSON.stringify(JSON.parse(xhr.responseText), null, 2);
          } catch (e) {
            rs += "Response body is not JSON" + eol;
          }
        }

        let title = request.method + " " + request.uri;
        title += " - " + xhr.status + " " + xhr.statusText;

        /* Add this item to the beginning of the array and truncate if necessary. */
        state.requestLog.unshift({ title: title, request: rq, response: rs });
        if (state.requestLog.length > state.requestLogMaxEntries) {
          state.requestLog.length = state.requestLogMaxEntries;
        }
      });
    });

    return response;
  }

  function enableView(view) {
    $("#" + view + "-view").addClass("visible");
    $("#" + view + "-link").addClass("active");
  }

  function formatCurrency(amt) {
    const formatter = new Intl.NumberFormat('en-US', {
      style: 'currency',
      currency: 'USD',
      minimumFractionDigits: 2
    })

    return formatter.format(amt);
  }

  /* Format as yyyy/mm/dd hh:mm:ss am/pm */
  function formatDateTime(dateTime) {

    let date = new Date(dateTime);

    let dateString = date.getFullYear() + "/"; 
    dateString += ("0" + (date.getMonth() + 1)).slice(-2) + "/";
    dateString += ("0" + date.getDate()).slice(-2) + " ";

    let h = date.getHours();
    let ap = "AM";
    if (h > 12) {
      h -= 12;
      ap = "PM";
    } else if (h == 0) {
      h = 12;
    }
    dateString += ("0" + h).slice(-2) + ":";
    dateString += ("0" + date.getMinutes()).slice(-2) + ":";
    dateString += ("0" + date.getSeconds()).slice(-2) + " " + ap;

    return dateString
  }

  function getTemplate(id) {
    let t = $(id).clone();
    t.removeClass("template");
    t.removeAttr("id");
    return t;
  }

  function go(path) {
    /* Default path is accounts. */
    if (typeof path === "undefined") {
      path = "accounts";
    }
    /* If the path already has a leading slash, remove it. */
    if (path.charAt(0) === '/') {
      path = path.substring(1);
    }
    window.location = "./#/" + path;
    $(window).trigger('hashchange');
  }

  function maskCardNumber(cardNumber) {
    return cardNumber.replace(new RegExp(/(\d{4}).*(\d{4})/, "gm"), "$1************$2");
  }

  function onHashChange(path) {

    /* Hide the current view and deactivate the current nav link. */
    $("main .view").removeClass("visible");
    $("#sidebar .nav-link").removeClass("active");

    /* The view is the first segment of the path. Default to the accounts view. */
    let view;
    if (typeof path !== "undefined") {
      let segments = path.split("/");
      if (segments.length >= 2) {
        view = segments[1].toLowerCase();
      }
    }

    switch (view) {
      case "account":
        viewAccount(path);
        break;
      case "card":
        viewCard(path);
        break;
      case "cards":
        viewCards();
        break;
      case "payment":
        viewPaymentTerminal();
        break;
      case "transaction":
        viewTransaction(path);
        break;
      case "transactions":
        viewTransactions();
        break;
      case "requestlog":
        viewRequestLog();
        break;
      case "settings":
        viewSettings();
        break;
      case "about":
        viewAbout();
        break;
      case "accounts":
      default:
        viewAccounts();
        break;
    }
  }

  async function onPaymentCardSelected(selected) {

    let response = await doApiRequest({ method: "GET", uri: "/acme/card/" + selected.item.value });

    $("#payment-view #cardNumber").val(response.json.cardNumber);
    $("#payment-view #nameOnCard").val(response.json.cardDetail.nameOnCard);
    $("#payment-view #expMonth").val(response.json.cardDetail.expMonth);
    $("#payment-view #expYear").val(response.json.cardDetail.expYear);
    $("#payment-view #securityCode").val(response.json.cardDetail.securityCode);
  }

  function parseCurrency(val) {
    val = val.replace(/[^0-9\.]+/g, "");
    return Number(val);
  }

  function raiseAlert(type, text) {
    let cl = (type === "success" ? "alert-success" : "alert-danger");

    $("#alert").append("<div>" + text + "</div>");
    let a = $("#alert > div");
    a.addClass("alert alert-dismissable fade show");
    a.addClass(cl);
    a.attr("role", "alert");
    setTimeout(function () {
      a.alert('close');
    }, 4000);
  }

  function responseCodeToString(code) {
    return code + " - " + responseCodeStrings[code];
  }

  async function saveAccount() {

    let json = {};
    let accountNumber = $("#account-view #accountNumber").val();
    let isUpdate = false;

    if (typeof accountNumber !== "undefined" && accountNumber.length > 0) {
      json.accountNumber = accountNumber;
      isUpdate = true;
    }

    json.accountDetail = {};
    json.accountDetail.firstName = $("#account-view #firstName").val();
    json.accountDetail.lastName = $("#account-view #lastName").val();
    json.accountDetail.address1 = $("#account-view #address1").val();
    json.accountDetail.address2 = $("#account-view #address2").val();
    json.accountDetail.city = $("#account-view #city").val();
    json.accountDetail.state = $("#account-view #state").val();
    json.accountDetail.postalCode = $("#account-view #postalCode").val();
    json.accountDetail.emailAddress = $("#account-view #email").val();
    json.accountDetail.phoneNumber = $("#account-view #phone").val();

    let request = { method: (isUpdate ? "PUT" : "POST"), uri: "/acme/account", json: json };
    if (isUpdate) {
      request.uri += "/" + accountNumber;
    }

    let response = await doApiRequest(request);

    if (response.status !== 200 && response.status !== 201) {
      raiseAlert("error", response.json.errorMessage);
      return;
    }

    let account = response.json;

    raiseAlert("success", "Account " + account.accountNumber + " has been " + (isUpdate ? "updated." : "created."));
    viewAccount("/account/" + account.accountNumber);
  }

  async function saveCard() {

    let json = {};
    let cardNumber = $("#card-view #cardNumber").val();
    let isUpdate = false;

    if (typeof cardNumber !== "undefined" && cardNumber.length > 0) {
      json.cardNumber = cardNumber;
      isUpdate = true;
    }

    json.cardDetail = {};
    json.cardDetail.nameOnCard = $("#card-view #nameOnCard").val();
    json.cardDetail.accountNumber = $("#card-view #accountNumber").val();
    json.cardDetail.expMonth = $("#card-view #expMonth").val();
    json.cardDetail.expYear = $("#card-view #expYear").val();
    json.cardDetail.securityCode = $("#card-view #securityCode").val();
    json.cardDetail.balance = parseCurrency($("#card-view #balance").val());
    json.cardDetail.spendingLimit = parseCurrency($("#card-view #spendingLimit").val());
    json.cardDetail.alertLimit = parseCurrency($("#card-view #alertLimit").val());
    json.cardDetail.isLocked = $("#card-view #isLocked").prop("checked");

    let request = { method: (isUpdate ? "PUT" : "POST"), uri: "/acme/card", json: json };
    if (isUpdate) {
      request.uri += "/" + cardNumber;
    }

    let response = await doApiRequest(request);

    if (response.status !== 200 && response.status !== 201) {
      raiseAlert("error", response.json.errorMessage);
      return;
    }

    let card = response.json;

    raiseAlert("success", "Card " + card.cardNumber + " has been " + (isUpdate ? "updated." : "created."));
    viewCard("/card/" + card.cardNumber);
  }

  async function saveSettings() {
    let host = $("#settings-view #host").val();

    /* We could use a complex regex to validate the URL, but this is a demo so let's go with
    garbage in, garbage out. */
    state.settings.baseUrl = host;
    raiseAlert("success", "The settings have been updated.");
  }

  async function submitPayment() {

    let json = {};
    json = {};

    json.cardNumber = $("#payment-view #cardNumber").val();
    json.nameOnCard = $("#payment-view #nameOnCard").val();
    json.expMonth = $("#payment-view #expMonth").val();
    json.expYear = $("#payment-view #expYear").val();
    json.securityCode = $("#payment-view #securityCode").val();
    json.amount = parseCurrency($("#payment-view #amount").val());
    json.merchantName = $("#payment-view #merchantName").val();

    let request = { method: "POST", uri: "/acme/payment", json: json };

    let response = await doApiRequest(request);

    if (response.status !== 201) {
      raiseAlert("error", "Declined: " + responseCodeToString(response.json.paymentResponseCode));
      return;
    }

    raiseAlert("success", "Payment completed");

    clearPaymentForm();

    go("/transaction/" + response.json.transactionId);
  }

  function transactionTypeToString(tx) {
    let s;
    if (tx.transactionType === 1) {
      s = "Sale";
      if (tx.relatedTransactionId.length !== 0) {
        s += " (Voided)"
      }
    } else {
      s = "Void";
    }
    return s;
  }

  function viewAbout(view, uri) {
    enableView("about");
  }

  async function viewAccount(path) {

    /* Spit the path. */
    let segments;
    try {
      segments = path.split("/");
    } catch (e) {
      go("accounts");
      raiseAlert("error", "Invalid account path");
      return;
    }

    /* The account number is the second segment in the path. If there isn't one, then 
    this is a create request. */
    let accountNumber = segments[2];

    if (typeof accountNumber === "undefined") {
      /* Setup the create account header, clear the form fields, and hide the delete button and card list. */
      $("#account-view #header").text("Create Account");
      $("#account-view :input").val("");
      $("#delete-account-button").hide();
      $("#account-view #account-cards").hide();
    } else {
      /* Set the header and turn on the delete button and card list. */
      $("#account-view #header").text("Account " + accountNumber);
      $("#delete-account-button").show();
      $("#account-view #account-cards").show();

      /* Load the account into the form. */
      let accountRequest = { method: "GET", uri: "/acme/account/" + accountNumber };
      let cardRequest = { method: "GET", uri: "/acme/cards?account-number=" + accountNumber };
      let [accountResponse, cardsResponse] = await Promise.all([doApiRequest(accountRequest), doApiRequest(cardRequest)]);

      if (accountResponse.status !== 200 || cardsResponse.status != 200) {
        raiseAlert("error", "Unable to load the account.");
        go("accounts");
        return;
      }

      let account = accountResponse.json;
      let cards = cardsResponse.json;

      $("#account-view #accountNumber").val(account.accountNumber);
      $("#account-view #firstName").val(account.accountDetail.firstName);
      $("#account-view #lastName").val(account.accountDetail.lastName);
      $("#account-view #address1").val(account.accountDetail.address1);
      $("#account-view #address2").val(account.accountDetail.address2);
      $("#account-view #city").val(account.accountDetail.city);
      $("#account-view #state").val(account.accountDetail.state);
      $("#account-view #postalCode").val(account.accountDetail.postalCode);
      $("#account-view #email").val(account.accountDetail.emailAddress);
      $("#account-view #phone").val(account.accountDetail.phoneNumber);

      let tbody = $("#account-view #account-cards > tbody");
      tbody.empty();

      if (cards.cards.length === 0) {
        tbody.append("<tr><td colspan=\"6\">There are no cards associated with this account.</td></tr>");
      } else {
        cards.cards.forEach((item) => {
          let row = getTemplate("#account-cards-row-template");

          $("td:nth-child(1)", row).text(item.cardNumber);
          $("td:nth-child(2)", row).text(item.cardDetail.nameOnCard);
          $("td:nth-child(3)", row).text(formatCurrency(item.cardDetail.balance));
          $("td:nth-child(4)", row).text(formatCurrency(item.cardDetail.spendingLimit));
          $("td:nth-child(5)", row).text(formatCurrency(item.cardDetail.alertLimit));
          $("td:nth-child(6)", row).text(item.cardDetail.isLocked);

          row.attr("data-href", "./#/card/" + item.cardNumber);
          tbody.append(row);
        });
      }

      $("#account-view #account-cards [data-href]").on("click", function () {
        window.location = $(this).data("href");
      });
    }

    enableView("account");
  }

  async function viewAccounts() {

    let response = await doApiRequest({ method: "GET", uri: "/acme/accounts" });

    if (response.status !== 200) {
      raiseAlert("error", "Unable to load the accounts list.");
      go("about");
      return;
    }

    let accounts = response.json.accounts;

    $("#accounts-table > tbody").empty();

    if (accounts.length === 0) {
      $("#accounts-table").append("<tr><td colspan=\"7\">There are no accounts on file.</td></tr>");
      enableView("accounts");
      return;
    }

    accounts.forEach((item) => {
      let row = getTemplate("#account-table-row-template");

      $("td:nth-child(1)", row).text(item.accountNumber);
      $("td:nth-child(2)", row).text(item.accountDetail.firstName);
      $("td:nth-child(3)", row).text(item.accountDetail.lastName);
      $("td:nth-child(4)", row).text(item.accountDetail.address1);
      $("td:nth-child(5)", row).text(item.accountDetail.city);
      $("td:nth-child(6)", row).text(item.accountDetail.state);
      $("td:nth-child(7)", row).text(item.accountDetail.postalCode);

      row.attr("data-href", "./#/account/" + item.accountNumber);

      $("#accounts-table > tbody").append(row);
    });

    $("#accounts-table [data-href]").on("click", function () {
      window.location = $(this).data("href");
    });

    enableView("accounts");
  }

  async function viewCard(path) {

    /* Spit the path. */
    let segments;
    try {
      segments = path.split("/");
    } catch (e) {
      go("cards");
      raiseAlert("error", "Invalid card path");
      return;
    }

    /* The card number is the second segment in the path. If there isn't one, then 
    this is a create request. */
    let cardNumber = segments[2];

    /* Populate the account select list. */
    let response = await doApiRequest({ method: "GET", uri: "/acme/accounts" });

    if (response.status !== 200) {
      raiseAlert("error", "Unable to retrieve the accounts list.");
      go("cards");
      return;
    }

    let accounts = response.json.accounts;

    let select = $("#card-view #accountNumber");
    select.empty();
    select.append("<option value=\"\">- select an account -</option>");
    accounts.forEach((item) => {
      let o = "<option value=\"" + item.accountNumber + "\">"
      o += item.accountNumber + " - " + item.accountDetail.firstName + " " + item.accountDetail.lastName;
      o += "</option>";
      select.append(o);
    });

    if (typeof cardNumber === "undefined") {
      /* Setup the create account header, clear the form fields, and hide the delete button. */
      $("#card-view #header").text("Create Card");
      $("#card-view :input").val("");
      $("#delete-card-button").hide();
      $("#card-view #form-balance").hide();
    } else {
      /* Set the header and turn on the delete button. */
      $("#card-view #header").text("Card " + cardNumber);
      $("#delete-card-button").show();
      $("#card-view #form-balance").show();

      /* Load the account into the form. */
      let response = await doApiRequest({ method: "GET", uri: "/acme/card/" + cardNumber });

      if (response.status !== 200) {
        raiseAlert("error", response.json.errorMessage);
        go("cards");
        return;
      }

      let card = response.json;

      $("#card-view #cardNumber").val(card.cardNumber);
      $("#card-view #nameOnCard").val(card.cardDetail.nameOnCard);
      $("#card-view #accountNumber").val(card.cardDetail.accountNumber);
      $("#card-view #expMonth").val(card.cardDetail.expMonth);
      $("#card-view #expYear").val(card.cardDetail.expYear);
      $("#card-view #securityCode").val(card.cardDetail.securityCode);
      $("#card-view #balance").val(formatCurrency(card.cardDetail.balance));
      $("#card-view #spendingLimit").val(formatCurrency(card.cardDetail.spendingLimit));
      $("#card-view #alertLimit").val(formatCurrency(card.cardDetail.alertLimit));
      $("#card-view #isLocked").attr("checked", card.cardDetail.isLocked);
    }

    enableView("card");
  }

  async function viewCards() {

    let response = await doApiRequest({ method: "GET", uri: "/acme/cards" });

    /* TODO: check for 200 */

    let cards = response.json.cards;

    let tbody = $("#cards-view #cards-table > tbody");

    tbody.empty();

    if (cards.length === 0) {
      tbody.append("<tr><td colspan=\"7\">There are no cards on file.</td></tr>");
      enableView("cards");
      return;
    }

    cards.forEach((item) => {
      let row = getTemplate("#card-table-row-template");

      $("td:nth-child(1)", row).text(item.cardNumber);
      $("td:nth-child(2)", row).text(item.cardDetail.nameOnCard);
      $("td:nth-child(3)", row).text(item.cardDetail.accountNumber);
      $("td:nth-child(4)", row).text(formatCurrency(item.cardDetail.balance));
      $("td:nth-child(5)", row).text(formatCurrency(item.cardDetail.spendingLimit));
      $("td:nth-child(6)", row).text(formatCurrency(item.cardDetail.alertLimit));
      $("td:nth-child(7)", row).text(item.cardDetail.isLocked);

      row.attr("data-href", "./#/card/" + item.cardNumber);

      tbody.append(row);
    });

    $("#cards-view #cards-table [data-href]").on("click", function () {
      window.location = $(this).data("href");
    });

    enableView("cards");
  }

  async function viewTransaction(path) {

    /* Spit the path. */
    let segments;
    try {
      segments = path.split("/");
    } catch (e) {
      go("transactions");
      raiseAlert("error", "Invalid transaction path");
      return;
    }

    let transaction = segments[2];

    let response = await doApiRequest({ method: "GET", uri: "/acme/transaction/" + transaction });

    if (response.status !== 200) {
      raiseAlert("error", response.json.errorMessage);
      go("transactions");
      return;
    }

    let tx = response.json;

    $("#transaction-view #transactionId").text(tx.transactionId);
    $("#transaction-view #date").text(new Date(tx.timestamp).toLocaleString());
    $("#transaction-view #type").text(transactionTypeToString(tx));
    $("#transaction-view #responseCode").text(responseCodeToString(tx.paymentResponseCode));
    $("#transaction-view #cardNumber").text(tx.paymentDetail.cardNumber);
    $("#transaction-view #amount").text(formatCurrency(tx.paymentDetail.amount));
    $("#transaction-view #merchantName").text(tx.paymentDetail.merchantName);
    $("#transaction-view #cardNumber").text(maskCardNumber(tx.paymentDetail.cardNumber));
    $("#transaction-view #nameOnCard").text(tx.paymentDetail.nameOnCard);
    $("#transaction-view #exp").text(tx.paymentDetail.expMonth + " / " + tx.paymentDetail.expYear);
    $("#transaction-view #securityCode").text(tx.paymentDetail.securityCode);

    let el = $("#transaction-view #relatedTransactionId");
    el.removeData("href");
    el.removeAttr("data-href");
    if (tx.relatedTransactionId.length > 0) {
      el.text(tx.relatedTransactionId);
      el.attr("data-href", "./#/transaction/" + tx.relatedTransactionId);
      el.on("click", function () {
        window.location = $(this).data("href");
      });
    } else {
      el.text("<none>");
    }

    if (tx.transactionType === 1 && tx.relatedTransactionId.length === 0 && tx.paymentResponseCode == "00") {
      $("#transaction-view #void-payment-button").show();
    } else {
      $("#transaction-view #void-payment-button").hide();
    }

    enableView("transaction");
  }

  async function viewTransactions() {

    let response = await doApiRequest({ method: "GET", uri: "/acme/transactions" });

    if (response.status !== 200) {
      raiseAlert("error", "An error occured while trying to retrieve the transaction list");
      go("cards");
      return;
    }

    let transactions = response.json.transactions;

    /*
    * In order to user jQuery DataTable without getting too complicated, we completely rebuild the
    * table every time. DataTable cannot deal with a DOM table that changes dynamically. Using a DataTable
    * data source would work well, but we're trying to keep things simple.
    */
    $("#transactions-table_wrapper").remove();
    $("#transactions-table").remove();
    let table = getTemplate("#transactions-table-template");
    table.attr("id", "transactions-table");
    $("#transaction-table-container").append(table);

    let tbody = $("tbody", table);

    if (transactions.length === 0) {
      tbody.append(getTemplate("#transactions-log-empty-template"));
      enableView("transactions");
      return;
    }

    transactions.forEach((item) => {
      let row = getTemplate("#transactions-table-row-template");
  
      $("td:nth-of-type(1)", row).text(formatDateTime(item.timestamp));
      $("td:nth-of-type(2)", row).text((item.transactionId));
      $("td:nth-of-type(3)", row).text(transactionTypeToString(item));
      $("td:nth-of-type(4)", row).text(formatCurrency(item.paymentDetail.amount));
      $("td:nth-of-type(5)", row).text(item.paymentDetail.merchantName);
      $("td:nth-of-type(6)", row).text(maskCardNumber(item.paymentDetail.cardNumber));
      $("td:nth-of-type(7)", row).text(responseCodeToString(item.paymentResponseCode));

      row.attr("data-href", "./#/transaction/" + item.transactionId);

      tbody.append(row);
    });

    $("#transactions-view #transactions-table [data-href]").on("click", function () {
      window.location = $(this).data("href");
    });

    $('#transactions-table').DataTable({ order: [[0, "desc"]] });

    enableView("transactions");
  }

  async function viewPaymentTerminal() {

    /* Populate the card number autocomplete with cards. */
    let response = await doApiRequest({ method: "GET", uri: "/acme/cards" });
    let cards = response.json.cards.map((item) => {
      return { label: item.cardNumber + " - " + item.cardDetail.nameOnCard, value: item.cardNumber };
    });
    $("#payment-view #cardNumber").autocomplete("option", "source", cards);

    enableView("payment");
  }

  function viewRequestLog() {

    let log = $("#request-log");

    log.empty();

    if (state.requestLog.length == 0) {
      log.append(getTemplate("#request-log-empty-template"));
      return;
    }

    state.requestLog.forEach((item, i) => {
      let t = getTemplate("#request-log-item-template");

      $("#heading-i", t).attr("id", "heading-" + i);
      $("#collapse-i", t).attr("id", "collapse-" + i);
      $("[data-target=\"#collapse-i\"]", t).attr("data-target", "#collapse-" + i);
      $("[aria-labelledby=\"heading-i\"]", t).attr("aria-labelledby", "heading-" + i);
      $("[aria-controls=\"collapse-i\"]", t).attr("aria-controls", "collapse-" + i);
      $("button", t).text(item.title);
      $(".card-body pre:nth-of-type(1)", t).text(item.request);
      $(".card-body pre:nth-of-type(2)", t).text(item.response);

      log.append(t);
    });

    enableView("requestlog");
  }

  function viewSettings() {

    $("#settings-view #host").val(state.settings.baseUrl);

    enableView("settings");
  }

  async function voidPayment() {

    let transactionId = $("#transaction-view #transactionId").text();

    let request = { method: "POST", uri: "/acme/payment/" + transactionId + "/void" };

    let response = await doApiRequest(request);

    if (response.status !== 200) {
      raiseAlert("error", response.json.errorMessage);
      return;
    }

    raiseAlert("success", "Transaction " + transactionId + " has been voided.");

    go("/transaction/" + response.json.transactionId);
  }

});
