App = {
  web3Provider: null,
  contracts: {},
  account: '0x0',
  assetsShown: {}, // Dict keeping track of which assets have been shown

  init: function() {
    return App.initWeb3();
  },

  initWeb3: function() {
    if (typeof web3 !== 'undefined') {
      // If a web3 instance is already provided by Meta Mask.
      App.web3Provider = web3.currentProvider;
      web3 = new Web3(web3.currentProvider);
    } else {
      // Specify default instance if no web3 instance provided
      App.web3Provider = new Web3.providers.HttpProvider('http://localhost:7545');
      web3 = new Web3(App.web3Provider);
    }
    return App.initContract();
  },

  render: function() {
    var sharedAssetsInstance;
    var loader = $("#loader");
    var content = $("#content");
  
    loader.show();
    content.hide();
  
    // Load account data
    web3.eth.getCoinbase(function(err, account) {
      if (err === null) {
        App.account = account;
        $("#accountAddress").html("Your Account: " + account);
      }
    });
  
    // Load contract data
    App.contracts.SharedAssets.deployed().then(function(instance) {
      saInstance = instance;
      return saInstance.assetCount();
    }).then(function(assetCount) {
      var assetsAvailable = $("#assetsResults");
      assetsAvailable.empty();
  
      var assetSelect = $('#assetSelect');
      assetSelect.empty();

      App.assetsShown = {} // We just erased all assets off the screen
  
      for (var i = 1; i <= assetCount; i++) {
        saInstance.assets(i).then(function(asset) {
          // if the asset has been shown, don't show it
          if (App.assetsShown[i]) return;

          // indicate that we're showing the asset
          App.assetsShown[i] = true;
          var id = asset[0];
          var name = asset[1];
          var cost = asset[2];
          var hasOut = asset[3];
          var checkTime = asset[4];
  
          // Render assets
          var assetTemplate = 
            "<tr><th>" + name + 
            "</th><td>" + cost/(1e18) + 
            "</td><td>" + hasOut + 
            "</td><td>" + new Date(checkTime*1000).toLocaleString() + "</td></tr>";
          assetsAvailable.append(assetTemplate);
  
          // Render asset options
          var assetOption = "<option value='" + id + "' >" + name + "</ option>";
          assetSelect.append(assetOption);
        });
      }
      // The following from the election example code
      //return electionInstance.voters(App.account);
      loader.hide();
      content.show();
    }).catch(function(error) {
      console.warn(error);
    });
  },

  
  checkOut: function() {
    var assetId = $('#assetSelect').val();
    App.contracts.SharedAssets.deployed().then(function(instance) {
      return instance.checkOutAsset.sendTransaction(assetId, Math.round((new Date()).getTime() / 1000), { from: App.account });
    }).then(function(result) {
      // Wait for assets to update
      $("#content").hide();
      $("#loader").show();
    }).catch(function(err) {
      console.error(err);
    });
  },
  
  checkIn: function() {
    var assetId = $('#assetSelect').val();
    App.contracts.SharedAssets.deployed().then(function(instance) {
      
      instance.assets(assetId).then(function(asset) {
        var cost = asset[2];
        console.log("cost", cost);
      });
      
      return instance.checkInAsset(assetId, Math.round((new Date()).getTime() / 1000), { from: App.account });
    }).then(function(result) {
      // Wait for assets to update
      $("#content").hide();
      $("#loader").show();
    }).catch(function(err) {
      console.error(err);
    });
  },
  
  listenForEvents: function() {
    App.contracts.SharedAssets.deployed().then(function(instance) {
      instance.changedEvent({}, {
        // Commented out so that we don't show every event
        //fromBlock: 0,
        //toBlock: 'latest'
      }).watch(function(error, event) {
        console.log("event triggered", event)
        // Reload when something changes
        App.render();
      });
    });
  },

  initContract: function() {
    $.getJSON("SharedAssets.json", function(sharedAssets) {
      // Instantiate a new truffle contract from the artifact
      App.contracts.SharedAssets = TruffleContract(sharedAssets);
      // Connect provider to interact with contract
      App.contracts.SharedAssets.setProvider(App.web3Provider);
  
      App.listenForEvents();
  
      return App.render();
    });
  }

};

$(function() {
  $(window).load(function() {
    App.init();
  });
});