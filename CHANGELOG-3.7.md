The entries below contain brief descriptions of the changes in a release, in no
particular order. Some of the entries reflect significant new additions, while
others represent minor corrections. Although this list is not a comprehensive
report of every change we made in a release, it does provide details on the
changes we feel Ice users might need to be aware of.

We recommend that you use the release notes as a guide for migrating your
applications to this release, and the manual for complete details on a
particular aspect of Ice.

- [Changes in Ice 3.7.0](#changes-in-ice-370)
  - [General Changes](#general-changes)

# Changes in Ice 3.7.0

These are the changes since Ice 3.6.1.

## General Changes

- IceGrid and IceStorm now use LMDB for their persistent storage instead of
  Freeze/BerkeleyDB.

- Added command line tools, `icegriddb` and `icestormdb`, to import/export the
  IceGrid and IceStorm databases.

- Added support for two additional IceGrid variables: `server.data` and
  `service.data`. These variables point to server and service specific data
  directories created by IceGrid on the node. These data directories are
  automatically removed by IceGrid if you remove the server from the
  deployment.

  For consistency, the `node.datadir` variable has been deprecated, use the
  `node.data` variable instead.

- Added the new metadata tag `delegate` for local interfaces with one operation.
  Interfaces with this metadata will be generated as a `std::function` in C++11,
  `delegate` in C#, `FunctionalInterface` in Java, `function callback` in
  JavaScript, `block` in Objective-C, `function/lambda` in Python. Other language
  mappings keep their default behavior.

- `ObjectFactory` has been deprecated in favor of the new local interface
  `ValueFactory`. Communicator operations `addObjectFactory`and
  `findObjectFactory` have been deprecated in favor of `addValueFactory` and
  `findValueFactory`.
