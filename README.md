# Team Socket's Search Engine - EECS 398 F19 Major Design Experience

This repository contains the source code for our capstone project where we built a search engine.

## Repo Structure

* `src/` contains the source code for each major component (crawler, parser, indexer, constraint solver, query compiler, ranker, frontend, and the drivers needed to run them), as well as their unit tests.

* `tst/` contains the Catch2 testing framework as well as supplementary files used in the unit tests

* `scripts/` contains bash scripts used to automate installations, file transfers, and running our drivers

* `data/` contains the necessary files to start crawling from scratch

## Running the Engine

### Prerequisites

We require `make` and `g++` to be installed for all components.

To run the crawler, LibreSSL and `s3fs` must also be installed, which can be accomplished with
```
sudo ./scripts/setEnvironment.sh
source ~/.bashrc
sudo ./scripts/libressl-install.sh
```

### Tests
To run all of the tests, run `make test` from the project's root directory. To run a specific test, use a commad like `make test case=src/utils/vector` (this will use the test file `src/utils/vector.test.cpp`).

### Crawler

Create a file called `data/instanceInfo.txt` in SearchEngine. The file will contain the number of machines that are being run in tandem, as well as the ID of this specific instance. The machine ID should range between 0 and the number of machines being run minus one. This will ensure that the seperate crawlers will not crawl the same links. The format of the file should be as follows:
```
numInstances=<number of machines>
instanceId=<this machine's ID>
```

Create a folder called socket-html next to your SearchEngine folder. This will contain the HTML that the crawler finds. Finally, run `source scripts/startCron.sh`, and you're done! It should be running.

The way this whole thing works is by running a cron job in the background that periodically checks to see if a crawler process is running. If no crawler process is running, cron starts a new crawler. We know that the crawler process will die every once in a while because of Amazon killing it or due to segfaults, but this cron job ensures that our driver process will be up and running most of the time.

Don't be worried if it segfaults/throws an error and stops. We expect this to happen every 5 to 20 minutes, although it's happened as soon as 30 seconds in. The cron job will make sure that the driver will be brought back every minute. If you think that something is broken, you can use the `date` command to see when a new minute passes. If a minute passes and by 30 seconds running the script called getDriverPID doesn't return anything, something went wrong.

Sanity Checks:
* Check your `~/.bashrc` file to see if you have
  ```
  # User specific aliases and functions
  export LD_LIBRARY_PATH=/opt/libressl/lib
  export LDFLAGS=/opt/libressl/lib
  export CPPFLAGS=/opt/libressl/include
  export PATH_TO_SEARCH_ENGINE=/path/to/your/SearchEngine
  ```
* To verify that these environment variables are set, you can run `echo $PATH_TO_SEARCH_ENGINE`. It should print out `/path/to/your/SearchEngine`. If they are not set, you can try `. ~/.bashrc`, but you need to make sure environment variables are set on startup of the SSH session. Reconnect to verify.
* To verify that the cron job is running, run `crontab -e`. It should look like what is in `scripts/runOnCron.txt`.

### Frontend

The frontend is served on an http server that listens on the port number
specified by a command line argument. It also needs a directory passed in as
the second argument to specify where the indexChunks exist on the local
machine. Usage: `./build/server.exe <port> <path/to/index/chunks>`

In order to test the frontend you can build test index chunks and run the
server using them. `make test case=src/frontend/chunks tls=no; mkdir
data/indexChunks; mv test_* data/indexChunks/; make serve tls=no` to do so.

`make serve` will run automatically on port 8000 and with the indexChunks build
in `data/indexChunks/`.

To run the CLI version of our interface, first build with `make cli`, and then run with `./build/frontend/cli.exe path/to/index/chunk/directory`.

## Components

### Crawler

All aspects of the crawler live in `src/driver/` and `src/crawler/`. The files in `src/driver/` are more concerned with keeping the crawler running continuously, and the files in `src/crawler/` with the actual HTML retrieval.

In the `src/crawler/` folder, `crawler.hpp` sends a GET request to webpages (http by default) and will follow up to 10 redirects automatically. The crawler is "polite" according to a websites `robot.txt` file which is parsed and stored in our robot class (defined in `robot.hpp`). It has a default crawl-delay of ten seconds and does not visit paths that are "disallowed".

URLs are constantly passed between different parts of the crawler and parser, so we have also defined a class in `url.hpp`. Once we receive HTML from a website, we pass the URL and HTML to the parser, which returns us the outgoing links to add to the frontier. We restrict the amount of links going to the same domain so we don't get stuck in a certain corner of the internet.

In order to continuously crawl, we need to have a seedlist (stored in `data/`), and to save our work as we go. We define the crawler's driver and most of its helper functions to save our work and manage the frontier in the `src/driver/` folder.

The `driver.cpp` spins up 100 threads that are all crawler workers. We use the pthread library to create locks and to send unblock threads using conditional variables. The driver is also responsible for locating all of the necessary files for loading the saved work and starting the crawler up again.

`frontier.hpp` manages putting in and taking out links from the frontier. It also contains a recently seen set that restricts the crawler from going into continuous loops on a given website, and clears after becoming full (50,000 links).

Saving the HTML for the index, and saving the work done by the crawler is handled in `checkpointing.hpp` so the crawler doesn't have to restart every time it crashes.

### Parser
The parser code is contained in `src/parser/`. The `HTMLParser` class contains the methods used in text-based HTML parsing to extract links, anchor text, and document text. It follows a simple approach where tags are itertively identified by their positions in the document to determine the type of content contained within. Steps are taken to avoid parsing of content within script and comment tags.

An HTML document is parsed when the HTMLParser object is initialized through its constructor. This constructor requires the document's URL, its text in string format, and a boolean to indicate whether the crawler or indexer is creating this object. The boolean helps to avoid parsing of unnecessary content for the crawler and indexer, respectively.

### Indexer
The indexer code is all in `src/indexer/`. We build our index into chunks, which contain an inverse word index for a set of URLs. We also define index stream readers (ISRs) for reading through the chunks. These ISRs have functions `next`, `nextDocument`, `seek`, and `get`, which allow for quick navigation.

### Constraint Solver
The constraint solver lives in `src/constraintSolver/`. We define ISRs recursively for NOT, AND, OR, and PHRASE. These ISRs are used by the query compiler to construct advanced queries.

### Query Compiler
The source for the query compiler can be found in `src/queryCompiler/`. The code is broken into three groups:
* `tokenstream.cpp` and `tokenstream.hpp`, which are responsible for simple text manipulation functions (e.g. matching characters, determining whether a character is a symbol, etc.);
* `expression.cpp` and `expression.hpp`, which are responsible for making an internal parse tree;
* and `parser.cpp` and `parser.hpp`, which are responsible for constructing the parse tree, as well as providing an interface to the ranker.

In terms of the actual language, we support NOT (~), AND (&), OR (|), and PHRASE (""). There is also an option to choose between infix and prefix notations.

### Ranker
The source for the ranker can be found in `src/ranker`. This ranks any documents for the frontend that are returned by the constraint solver. Static ranking involves simple heuristics for the title and URL of the document. Dynamic ranking involves finding spans for the query in the document title and body and using a bag of words scoring strategy for the document body. Scoring a span takes into account the length and the ordering of the span. Spans currently must have all words in the query; partial spanning has not been implemented.
### Frontend

The frontend provides an entry-point for search queries. The server runs on `src/frontend/httpServer.cpp` and listens on port `8000`. It serves the HTML and CSS files for rendering the content of the search engine.

We provide an easter egg for a search query "Team Socket".
