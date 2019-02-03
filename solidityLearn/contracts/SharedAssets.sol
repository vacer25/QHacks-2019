pragma solidity ^0.5.0;

contract SharedAssets {
    struct Asset {
        uint32 id;
        string name;
        uint cost;
        address payable hasOut;
        uint64 checkTime;
    }

    mapping (uint32 => Asset) public assets;

    uint32 public assetCount;

    function addAsset (string memory _name, uint _cost) private {
        assetCount ++;
        assets[assetCount] = Asset(assetCount, _name, _cost, address(0), 0);
        //Should set time to the current blocktime or something
    }

    function checkOutAsset (uint32 _asset, uint64 _time) payable public {
        // Check asset number
        require(_asset > 0 && _asset <= assetCount);

        // Ensure that the asset is checked in (i.e. checked out by the null address)
        require(assets[_asset].hasOut == address(0));

        // Ensure the time makes sense
        require(assets[_asset].checkTime < _time && _time < block.timestamp);
        // Blockchains can't have secure timestamps!  But this works well enough for us...
        // We pass in a time parameter as opposed to just using block.timestamp, under the assumption that the lock (executing this function) knows the time at least slightly better than the blockchain

        // TODO: Handle payment??!!
        require(msg.value == assets[_asset].cost);

        // Actually do the thing
        assets[_asset].hasOut = msg.sender;
        assets[_asset].checkTime = _time;

        emit changedEvent();
    }

    function checkInAsset (uint32 _asset, uint64 _time) public {
        // Check asset number
        require(_asset > 0 && _asset <= assetCount);

        // Ensure that the asset is checked out by the person checking it in
        require(assets[_asset].hasOut != address(0));

        // ensure time makes sense
        require(assets[_asset].checkTime < _time && _time < block.timestamp);

        // TODO: payment
        assets[_asset].hasOut.transfer((assets[_asset].cost / 10) * 9);       // For now this just gives the money back
        // What about gas though...

        assets[_asset].hasOut = address(0);
        assets[_asset].checkTime = _time;
        
        emit changedEvent();
    }

    function getBalance() public view returns (uint) {
        return address(this).balance;
    }

    function sink() payable public{
        //Take the money
    }

    function() external payable {
        //Falback
    }

    event changedEvent (); //Do nothing, just to be listened for

    constructor() public {
        addAsset("Vacuum", 1 ether);
    }

}
