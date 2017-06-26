# Pattern Index - an index for fast lookup of patterns in the AtomSpace

This module provides two interfaces (C++ and Guile) for a data abstraction that
alows the user to create indexes for subsets of Atoms from the AtomSpace and
then submit queries to retrieve subgraphs that matches a given pattern.

_*NOTE*: this is not a wrapper around OpenCog's PatternMatcher._

## Why having a Pattern Index?

The motivation behind this implementation are the applications that use
Opencog's AtomSpace as data container. Such applications usually load a
(potentially huge) bunch of pre-processed information to the AtomSpace and then
perform some sort of computation using this information (eventually generating
more information which is stored back in the AtomSpace)

Using AtomSpace information may be time-expensive specially if the number of
atoms involved is huge. So having an index to perform faster lookup of patterns
may be essential to allow the algorithms to run in an acceptable time.

So this module allows the user to do something like:

1. Load a large SCM with lots of atoms
1. Build an index with only the relevant atoms
1. Use this index to gather information from the AtomSpace as required by the algorithm
1. Optionally discard the index when it is no longer necessary (this will have no effect in the AtomSpace)

Although it's not the main purpose of this module, Pattern Index have
additional algorithms to perform pattern mining in the index patterns. Those
algorithms have been written according to the methodology described in these
documents:

* http://wiki.opencog.org/w/Pattern\_miner#Tutorial\_of\_running\_Pattern\_Miner\_in\_Opencog
* http://wiki.opencog.org/w/Measuring\_Surprisingness

_*NOTE*: this functionality is not a wrapper around OpenCog's PatternMiner_

## Additional motivation for the Pattern Index

The Pattern Index internal structure have been designed to allow building the
index in the disk (rather in in RAM) and without the need af having an actual
AtomSpace in memory.

This is relevant because one would be able to build an index (entirely in the
disk) given a huge SCM file which would never fit in RAM.  This functionality
IS NOT IMPLEMENTED YET but it is an fairly-easy-to-implement new feature.

Once we have this feature we would be able to do fairly fast lookup for
patterns in huge datasets in disk.

## Relevant files for potential users (examples and API)

If you plan to use this module you want to take a look at the following files:

* **patternIndexQueryExample.cc and patternIndexMiningExample.cc:** Small C++ programs with a complete use case of the Pattern Index being used to perform queries and pattern mining. If you plan to use Pattern Index from C++ code this is the right place to start.
* **pattern-index-query-example.scm and pattern-index-mining-example.scm:** Small Scheme programs with a complete use case of the Pattern Index being used to perform queries and pattern mining. If you plan to use Pattern Index from Scheme code this is the right place to start.
* **toy-example-query.scm and toy-example-mining.scm:** Datasets used by the example programs above
* **ExampleConfig.conf:** Opencog's configuration file used by the example programs above
* **PatternIndexAPI.h:** This is the C++ API.
* **PatternIndexSCM.{h,cc}:** This is the Scheme API.

## Other files (actual implementation of functionalities)

* **TypeFrameIndex.{h,cc}:** Actual implementation of all the index algorithms (creation, queries and mining)
* **TypeFrame.{h,cc}:** Basic data abstraction used in the index. It's a simplified representation of Atom
* **CartesianProductGenerator.{h,cc}, CombinationGenerator.{h,cc} and PartitionGenerator.{h,cc}:** Helper classes used to iterate through combinations of set(s) elements.
* **SCMLoader.{h,cc}, SCMLoaderCallback.{h,cc} and TypeFrameIndexBuilder.{h,cc}:** Helper classes used to load SCM files.

## How to use the Pattern Index

Reading the example programs mentioned above are the best way to understand how to use the Pattern Index.

### Creating a new index

#### C++

`
    Handle indexKey = patternindex().createIndex("toy-example-query.scm");
`

#### Scheme

`
    (define indexKey (pi-create-index (ConceptNode "toy-example-query.scm")))
`

indexKey will be used in further calls to query or mine the newly created
index. You can create many indexes using different sets of atoms and query them
separetely.

### Quering requests

We'll use "toy-example-query.scm" to illustrate queries examples.

`
    (SimilarityLink (ConceptNode "human") (ConceptNode "monkey"))
    (SimilarityLink (ConceptNode "human") (ConceptNode "chimp"))
    (SimilarityLink (ConceptNode "chimp") (ConceptNode "monkey"))
    (SimilarityLink (ConceptNode "snake") (ConceptNode "earthworm"))
    (SimilarityLink (ConceptNode "rhino") (ConceptNode "triceratops"))
    (SimilarityLink (ConceptNode "snake") (ConceptNode "vine"))
    (SimilarityLink (ConceptNode "human") (ConceptNode "ent"))
    (InheritanceLink (ConceptNode "human") (ConceptNode "mammal"))
    (InheritanceLink (ConceptNode "monkey") (ConceptNode "mammal"))
    (InheritanceLink (ConceptNode "chimp") (ConceptNode "mammal"))
    (InheritanceLink (ConceptNode "mammal") (ConceptNode "animal"))
    (InheritanceLink (ConceptNode "reptile") (ConceptNode "animal"))
    (InheritanceLink (ConceptNode "snake") (ConceptNode "reptile"))
    (InheritanceLink (ConceptNode "dinosaur") (ConceptNode "reptile"))
    (InheritanceLink (ConceptNode "triceratops") (ConceptNode "dinosaur"))
    (InheritanceLink (ConceptNode "earthworm") (ConceptNode "animal"))
    (InheritanceLink (ConceptNode "rhino") (ConceptNode "mammal"))
    (InheritanceLink (ConceptNode "vine") (ConceptNode "plant"))
    (InheritanceLink (ConceptNode "ent") (ConceptNode "plant"))
`

#### C++

In C++ one can query an index either passing a Handle or a std::string representing the query.

`
    std::string queryStr = "(AndLink (SimilarityLink (VariableNode \"X\") (VariableNode \"Y\")) (SimilarityLink (VariableNode \"Y\") (VariableNode \"Z\")))";
    Handle queryHandle = schemeEval->eval_h(queryStr);
    Handle resultHandle = patternindex().query(indexKey, queryHandle);
`

resultHandle will point to a ListLink with the query result. This link is a
list with all the satisfying subgraphs (each represented by a ListLink) and the
respective variable assignments (also represented by ListLinks to pairs
`<variable, assigned atom>`).

`
    (ListLink
        (ListLink
            (ListLink
                (SimilarityLink (ConceptNode "monkey") (ConceptNode "chimp"))
                (SimilarityLink (ConceptNode "monkey") (ConceptNode "human"))
            )
            (ListLink
                (ListLink (VariableNode "X") (ConceptNode "chimp"))
                (ListLink (VariableNode "Y") (ConceptNode "monkey"))
                (ListLink (VariableNode "Z") (ConceptNode "human"))
            )
        )
        (ListLink
            (ListLink
                (SimilarityLink (ConceptNode "ent") (ConceptNode "human"))
                (SimilarityLink (ConceptNode "monkey") (ConceptNode "human"))
            )
            (ListLink
                (ListLink (VariableNode "X") (ConceptNode "ent"))
                (ListLink (VariableNode "Y") (ConceptNode "human"))
                (ListLink (VariableNode "Z") (ConceptNode "monkey"))
            )
        )
        (ListLink
            (ListLink
                (SimilarityLink (ConceptNode "human") (ConceptNode "chimp"))
                (SimilarityLink (ConceptNode "ent") (ConceptNode "human"))
            )
            (ListLink
                (ListLink (VariableNode "X") (ConceptNode "ent"))
                (ListLink (VariableNode "Y") (ConceptNode "human"))
                (ListLink (VariableNode "Z") (ConceptNode "chimp"))
            )
        )
        (ListLink
            (ListLink
                (SimilarityLink (ConceptNode "earthworm") (ConceptNode "snake"))
                (SimilarityLink (ConceptNode "vine") (ConceptNode "snake"))
            )
            (ListLink
                (ListLink (VariableNode "X") (ConceptNode "vine"))
                (ListLink (VariableNode "Y") (ConceptNode "snake"))
                (ListLink (VariableNode "Z") (ConceptNode "earthworm"))
            )
        )
    )
`

Optionally one can pass the query in a std::string.  Using this syntax,
`query()` will populate a vector of QueryResult with all the subgraphs (and
respective variable assigments) that satisfies the passed pattern.

`
    std::string queryStr = "(AndLink (SimilarityLink (VariableNode \"X\") (VariableNode \"Y\")) (NotLink (AndLink (InheritanceLink (VariableNode \"X\") (VariableNode \"Z\")) (InheritanceLin
    std::vector<PatternIndexAPI::QueryResult> queryResult;
    patternindex().query(queryResult, indexKey, queryStr);
`

In our example, this query would return:

`
    Result #1:
    
    (SimilarityLink
        (ConceptNode "earthworm")
        (ConceptNode "snake")
    )
    
    Mapping:
    
    (VariableNode "X")
    (ConceptNode "earthworm")
    
    (VariableNode "Y")
    (ConceptNode "snake")
    
    Result #2:
    
    (SimilarityLink
        (ConceptNode "triceratops")
        (ConceptNode "rhino")
    )
    
    Mapping:
    
    (VariableNode "X")
    (ConceptNode "rhino")
    
    (VariableNode "Y")
    (ConceptNode "triceratops")
    
    Result #3:
    
    (SimilarityLink
        (ConceptNode "vine")
        (ConceptNode "snake")
    )
    
    Mapping:
    
    (VariableNode "X")
    (ConceptNode "snake")
    
    (VariableNode "Y")
    (ConceptNode "vine")
    
    Result #4:
    
    (SimilarityLink
        (ConceptNode "ent")
        (ConceptNode "human")
    )
    
    Mapping:
    
    (VariableNode "X")
    (ConceptNode "ent")
    
    (VariableNode "Y")
    (ConceptNode "human")
`

#### Scheme

In scheme we use only one syntax:

`
    (pi-query index-key (AndLink (SimilarityLink (VariableNode "X") (VariableNode "Y")) (SimilarityLink (VariableNode "Y") (VariableNode "Z"))))
`

Which return the same answer as the first syntax described above for C++.

### Mining requests

To illustrate the pattern mining using Pattern Index, we'll use
"toy-example-mining.scm" which is exactly the same dataset
`ugly_male_soda-drinker_corpus.scm` used as example in OpenCog's PatternMiner.

#### C++

`
    patternindex().minePatterns(resultPatterns, indexKey);
`

resultPatterns is populated with the best found patterns. Actually with pairs `<quality measure, pattern toplevel atom>`. In our example, the best pattern is:

`
    (AndLink
        (InheritanceLink
            (VariableNode "V0")
            (ConceptNode "ugly")
        )
        (InheritanceLink
            (VariableNode "V0")
            (ConceptNode "man")
        )
        (InheritanceLink
            (VariableNode "V0")
            (ConceptNode "soda drinker")
        )
    ) 
`


#### Scheme

`
    (pi-mine-patterns index-key)
`

Results are basically the same as the above but returned as a nested ListLink instead of a structured type.

## Running the example programs

To run the examples just execute one of these command lines (from `build/`):

`
./opencog/nlp/pattern-index/patternIndexQueryExample ../opencog/nlp/pattern-index/toy-example-query.scm ../opencog/nlp/pattern-index/ExampleConfig.conf
`

`
./opencog/nlp/pattern-index/patternIndexMiningExample ../opencog/nlp/pattern-index/toy-example-mining.scm ../opencog/nlp/pattern-index/ExampleConfig.conf
`

`
guile -l ../opencog/nlp/pattern-index/pattern-index-query-example.scm
`

`
guile -l ../opencog/nlp/pattern-index/pattern-index-mining-example.scm
`

_*NOTE*: The Scheme API read the configuration file in `lib/opencog.conf`._

## TODO

1. Implement C++ and Scheme API to allow creation of empty index followed by addition of atoms and finally actual creation of the index.
1. Implement multi-thread version of TypeframeIndex::query()
1. Implement optional creation of TypeFrameIndex pointing to elements in disk
1. Implement optional creation of TypeFrameIndex storing the index itself in disk
1. Implement distributed version of the mining algorithm
1. Implement distributed version of TypeFrameIndex

