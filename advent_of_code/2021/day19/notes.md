This one is probably taking longer.
The big-picture approach is relatively straight forward, but the steps might become complicated.

The general approach is roughly as follows:

* For each scanner, calculate the distances (or distance vectors) between all the beacons.
* Use these distances to find the overlapping regions. They are identical despite the different scanner orientations/positions.
* Start with the first scanner as the "origin".
* Find the scanner with at least 12 overlapping points.
* Determine the rotation and translation of the other scanner in relation to the origin based on the overlapping beacones.
* Rotate and translate the other scanner's beacons and add them to the origin.
* Repeat until all beacons/scanners have been added.
