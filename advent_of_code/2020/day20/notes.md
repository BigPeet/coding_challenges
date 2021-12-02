I assume that the solution is a square image.
The 9 example images in the task.md (see test\_input\_01.txt) are assembled into a 3x3 square.
The 144 images in the real input should form a 12x12 square.

I currently assume that every match is unambiguous, i.e. I don't suspect a tile will match another tile, but won't be matched with it in the assembled solution.
The rotation of the assembled square is ambiguous (there should be at least 4 possible solutions).

Tasks:
- Create "Image Type" that contains the data and offers methods to rotate (by 90 degrees) and flip (horizontically and/or vertically) the data.
- Create a "matches" method on the Image type that takes another image and compares the borders (4x2 borders (flipped)).

To get the solution for part 1, it would be sufficient to identify the corner images, i.e. the ones with only two matching borders.
But I assume that part 2 will require assembling the full image. I will decide if I do that preemptively or wait until I know the details of part 2.

I did the "lazy" solution for part 1 to find the corner pieces.
The solution for the first part is 28057939502729.

But now I have to actually assemble the image.

Ok, I've managed to assemble the image.
Now to find the "monsters".
I assume monsters do NOT overlap (for now).

The solution for the second part is 2489.
