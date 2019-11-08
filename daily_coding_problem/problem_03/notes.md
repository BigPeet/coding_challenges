A few notes on the implementation:

1. The (de-)serialization implemented here is vulnerable to ";", "[" and "]" in the value fields of the nodes.
   I am aware of that, but decided to not go further in this challenge.
   Possible solutions to circumvent this:
    1. Implement a function that "sanitizes" the value of each node when serializing, e.g. add an escape character in front of every ";" or change every "[" into "\BRACKET" or something.
       This still leaves the serialization open for a malicious user...so:
    2. Check the value for character sequences that will trip the implementation up beforehand and refuse them.

2. The deserialization might not only be victim to "reserved" characters in the value of nodes, but the input string might not be well-formed or a serialized node at all.
   I only performed limited checks for that and I guess a lot more could be added to prevent errors from random strings or strange corner cases.
