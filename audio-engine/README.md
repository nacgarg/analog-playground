# Audio Engine

This part of the project is the backend of a node-based audio processing environment similar to Max/MSP or PureData.

Some constraints in this project:

- Cycles are not allowed
- Only one connection per input/output jack (you can't have one output be connected to multiple inputs)
  - if you want to split an output to use it in multiple places you need to use a splitter module (WIP)
  - This ensures that all processing can be done in place

## Module

A Module is essentially a "node" within the audio graph. A derived Module can have multiple inputs (InputJacks) and outputs (OutputJacks). The AudioGraph will call its `process()` function after making sure that any inputs are processed. InputJacks and OutputJacks have a `connected` field that can be used to modify behavior or optimize a Module's `process()`, for example, if an independent output is not used it doesn't need to be computed, and if an input isn't connected then a module can revert to default values or something like an LFO.

