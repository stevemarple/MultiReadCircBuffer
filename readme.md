# MultiReadCircBuffer

## Introduction

MultiReadCircBuffer is an Arduino library implementing a multiple-reader
circular buffer. The library allows a single writer to insert data, with
multiple readers extracting data from the same shared buffer. The
implementation does not use malloc(), instead the caller must provide the
buffer space and length when creating the MultiReadCircBuffer object. If
more than one reader is required then the caller must also supply arrays
for the size and read pointer arrays. The caller can determine whether
data should be overwritten or not if it is inserted faster than the
slowest reader is removing it. Any buffer size between 1 and max int -1
is valid.

## Current status

Compiles under Arduino IDE. Behaviour (excluding enable/disable interrupt code) tested under g++.
Regression testing under g++.





