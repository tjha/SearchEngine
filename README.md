# SearchEngine
## Team Socket - EECS 398 F19 Major Design Experience

This repository contains the source code for our search engine which we
designed for our capstone project.

This is not a comprehensive readme. Our team is behind and is putting the pieces together. This will be updated as will the final report.

## Repo Structure

* `src` contains a directory of the source code for each major component
  (crawler, parser, indexer, constraint solver, query compiler, ranker,
  frontend, and the drivers needed to run them)

* `tst` contains the units test using the Catch2 framework for cpp

* `scripts` contains bash scripts used to automate installations, file
  transfers, and running our drivers

* `data` contains the necessary files to start crawling from scratch

### Crawler

All aspects of the crawler live in `src/driver/` and `src/crawler/`. The files
in `driver/` are more concerned with keeping the crawler running continuously,
and the files in `crawler/` with the actual HTML retrieval.

In the `crawler/` folder, `crawler.hpp` sends a GET request to webpages (http
by default) and will follow up to 10 redirects automatically. The crawler is 
"polite" according to a websites `robot.txt` file which is parsed and stored in
our robot class (defined in `robot.hpp`). It has a default crawl-delay of ten
seconds and does not visit paths that are "disallowed".

Urls are constantly passed between different parts of the crawler and parser,
so we have also defined a class in `url.hpp`. Once we receive HTML from a
website, we pass the url and html to the parser, which returns us the outgoing
links to add to the frontier. We restrict the amount of links going to the same
domain so we don't get stuck in a certain corner of the internet. 

In order to continuously crawl, we need to have a seedlist (stored in `data/`),
and to save our work as we go. We define the crawler's driver and most of its
helper functions to save our work and manage the frontier in the `src/driver/`
folder. 

The `driver.cpp` spins up 100 threads that are all crawler workers. We use the
pthread library to create locks and to send unblock threads using conditional
variables. The driver is also responsible for locating all of the necessary
files for loading the saved work and starting the crawler up again.

`frontier.hpp` manages putting in and taking out links from the frontier. It
also contains a recently seen set that restricts the crawler from going into
continuous loops on a given website, and clears after becoming full (50,000
links).

Saving the html for the index, and saving the work done by the crawler is
handled in `checkpointing.hpp` so the crawler doesn't have to restart every
time it crashes.


### Parser


### Indexer


### Constraint Solver 


### Query Compiler


### Ranker


### Frontend

The frontend provides an entry-point for search queries. The server runs on 
`src/frontend/httpServer.cpp` and listens on port 8000. It is an extension of
the server lab assignment and is modified to serve the HTML and CSS files for
rendering the content of the search engine.

We provide an easter egg for a search query "Team Socket".


