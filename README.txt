This is not a comprehensive readme. Our team is behind and is putting the pieces together. This will be updated as will the final report.

src contains a directory of the source code for each component. The query compiler should be merged in shortly after midnight Friday, so this should be updated soon. There is additionally a directory here for our utilities such as our UTF library, vector, string, and unorderedMap. driver is our process to run crawlers, manage the frontier, and use parsers. frontend, in addition to the front-end, contains the server which takes requests from the user and spins ranker threads to contact the constraint solver/query compiler workers for each indexChunk. There is a driver.cpp in indexer for creating indexChunks from downloaded HTML

tst is a directory of mirrored format. We used the Catch2 testing framework for our tests so they are automatically run upon pushes to our Github.

