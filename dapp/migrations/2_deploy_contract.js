var SharedAssets = artifacts.require("./SharedAssets.sol");

module.exports = function(deployer) {
    deployer.deploy(SharedAssets);
};