# Frequent Itemset algorithms

Using A-Priori Frequent Itemset algorithm to implement the randomised sample algorthm and SON (Savasere, Omiecinski, and Navathe)


Randomised Algorithm program executed with:
javac randomised.java && java Randomised {InputFile} {SupportThreshold} {SamplePercent} > Output.txt

SON Algorithm program executed with:
javac son.java && java Son {InputFile} {SupportThreshold} {ChunkFraction} > Output.txt

{SupportThreshold} as an int
{SamplePercent}/{ChunkFraction} as a decimal 0.0 - 1.0

Implementation:
Implementing the A-Priori algorithm
For the representation of the data I used the triples method in a hash table, where the keys are either the pairs of ints or tuples,
and the key is the count of that item.
During the analysis phase the hash table is iterated through and every element with a count higher than the support threshold is
saved/outputted.
This is changed slightly in the SON implementation where after each chunk the hash table is reset once the items with enough support
are saved.
