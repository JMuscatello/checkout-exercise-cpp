CHECKOUT_EXERCISE.CPP
=====================

Program to implement a simple supermarket checkout that includes two special offers.

The program reads an item index file that is specified at runtime. The index file must contain lines corresponding to each item in the format
<integer item code>     <item name (string)>    <item price>    <offer identifier (string)>
Each column must be separated by tabs.
I have supplied a sample index file "item_index.txt". The special offer identifiers are "3FOR2" corresponding to three for two offers for same type items and "3RDFREE" which corresponds to cheapest third item free.

usage:

$ ./checkout_exercise.x -f <path-to-index-file>

The program prompts the user to enter successive item codes or 'e' to end the current transaction and apply any offers. The user is then prompted to continue or end the session.

The code makes use of several parts of the STL. This program was tested and compiled on a linux system using the g++ compiler with -std=c++0x flag.


By Jordan Muscatello
