pragma solidity 0.5.0;
// Welp this tutorial is using an old version of solidity and lots of things are different!  This'll be fun

contract Election {
    // Read/write candidate
    string public candidate;
    //State variables allow us to write data to the blockchain
    // It's public so solidity gives us a built-in getter

    //Constructor
    constructor() public {
        candidate = "Candidate 1";
    }
}