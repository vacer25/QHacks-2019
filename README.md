# Diamond
**Secure and Transparent Asset Management System, Distributed Across the Ethereum Network**

QHacks-2019, troygiorshev, vacer25

## Inspiration

* **An Ethereum Smart Contract, and a Human Social Contract, _together_.**

* Apartments are getting smaller and smaller, so there's less space for bulky appliances, like vacuums.
* Instead, have a secure room with communal appliances that can be rented out.
* The problem with rental equipment is that people don't care, they tend to get misused as no one takes ownership.
* By leveraging the transparency of the Ethereum blockchain, we have integrated social consequences into rental equipment, ensuring they are treated with care.
* Anyone can see who last took out an item, and how well they treated it, giving an incentive to be responsible.

## What it does

* Appliances are fitted with an IoT smart device, which then interfaces with a lock
* The lock has a connection to the Ethereum Network
* When the device is unlocked with an RFID keycard it is recorded into the publicly viewable Ethereum Blockchain
* Then, when the device is returned, the Blockchain records the condition of the device
* This includes things like if it was dropped or damaged, or if it was left wasted on a shelf for weeks

## How we built it

* We used Ganache to host a private Ethereum Network, and used Truffle to deploy smart contracts.
* Remix was used extensively for offline testing and validation
* We built a smart IoT device using an Arduino, IMU and RFID modules, and an LCD screen which we soldered and hot-glued together
* The locker mechanism uses a servo motor to open and close the compartment which houses the IoT device
* It also uses two RFID modules: one to authenticate the user and unlock the asset, the other to communicate with the asset about its condition when it is returned.

## Challenges we ran into

* Working with Solidity and Ethereum for the first time proved to be quite challenging but in the end we got it to work
* Countless RFID challenges including reliably reading data and establishing connections

## Accomplishments that we're proud of

* It works!  The dapp (distributed app) successfully runs on the Ethereum network!
* Building both the IoT device and the locking device in such a short time
* Being able to communicate with the RFID modules and tags

## What we learned

* This was our first time creating an Ethereum project!  Solidity is a very interesting language, and there was a lot to learn to make things work.  ConsenSys's tools, like Truffle and Ganache made the learning process easier and faster.
* This was also our first time using RFID reader modules and exchanging data using this communication protocol

## What's next for Diamond

* Full deployment to the Ethereum rinkeby testnet through Infura.
* Further prototyping for the physical lock design.
