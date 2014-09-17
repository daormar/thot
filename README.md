Thot: a Toolkit for Statistical Machine Translation
===================================================
Thot is an open source toolkit for statistical machine
translation. Originally, Thot incorporated tools to train phrase-based
models. The new version of Thot now includes a state-of-the-art
phrase-based translation decoder as well as tools to estimate all of the
models involved in the translation process. In addition to this, Thot is
also able to incrementally update its models in real time after
presenting an individual sentence pair.

Thot is being developed by [Daniel Ortiz-Martínez] [1]. Daniel is a
member of the [PRHLT research group] [2] at the [Polytechnic University
of Valencia] [3].


Features
--------
The toolkit includes the following features:

- Phrase-based statistical machine translation decoder
- Computer-aided translation (post-editing and interactive machine translation)
- Incremental estimation of all of the models involved in the translation process
- Robust generation of alignments at phrase-level
- Client-server implementation of the translation functionality
- Single word alignment model estimation using the incremental EM algorithm
- Scalable implementation of the different estimation algorithms using Map-Reduce
- Compiles on Unix-like and Windows (using Cygwin) systems
- Integration with the [CasMaCat Workbench] [4] developed in the EU FP7 [CasMaCat project] [5]
- ...


Distribution Details
--------------------
Thot has been coded using C, C++ and shell-scripting. Thot is known to
compile on Unix-like and Windows (using Cygwin) systems. See the
"Support" section of this file if you experience problems during
compilation.

It is released under the [GNU Lesser General Public License (LGPL)] [6].


Installation
------------
To install Thot, first you need to install the autotools (autoconf,
autoconf-archive, and automake packages in Ubuntu). If you are planning
to use Thot on a Windows platform, you also need to install the Cygwin
environment.

Once the autotools are available (as well as Cygwin if required), you
can proceed with the installation of Thot by following the next sequence
of steps:

 1. Obtain the package using git:

        $ git clone https://github.com/daormar/thot.git

    Or [download it in a zip file] [7]

 2. `cd` to the directory containing the package's source code and type
    `./reconf`

 3. Type `./configure` to configure the package

 4. Type `make` to compile the package

 5. Type `make install` to install the programs and any data files and
    documentation

 6. You can remove the program binaries and object files from the source
    code directory by typing `make clean`

By default the files are installed under the /usr/local/ directory (or
similar, depending of the OS you use); however, since Step 5 requires
root privileges, another directory can be specified during Step 3 by
typing:

     $ configure --prefix=<absolute-installation-path>

For example, if "user1" wants to install the Thot package in the
directory /home/user1/thot, the sequence of commands to execute should be
the following:

     $ ./reconf
     $ configure --prefix=/home/user1/thot
     $ make
     $ make install

NOTE: the installation directory can be the same directory where the
Thot package was decompressed.

In order to build and use Thot on Windows platforms, the linux-like
environment called Cygwin must be downloaded and installed previously.

See "INSTALL" file for more information.


Installation Including the CasMaCat Workbench
---------------------------------------------
Thot can be combined with the CasMacat Workbench that is being developed
in the project of the same name. [See this webpage] [4] to get the specific
installation instructions.


Current Status
--------------
The Thot toolkit is under development. Original public versions of Thot
date back to 2005 [Ortiz-Martínez et al., 2005] and were hosted in
SourceForge. These original versions were strongly focused on the
estimation of phrase-based models. By contrast, current version offers
several new features that had not been previously incorporated.

A basic usage manual is currently being developed. In addition to this,
a set specific tools to ease the process of making SMT experiments has
been created.

In addition to the basic usage manual, there are some toolkit extensions
that will be incorporated in the next few months:

- Virtualized language models (i.e. accessing language model parameters
  from disk).

- Interpolation of language and translation models.

- Improved concurrency in the Thot translation server (translation
  process is not still concurrent).

Finally, here is a list of known issues with the Thot toolkit that are
currently being addressed:

- Phrase model training is based on HMM-based word alignment models
  estimated by means of incremental EM. The current implementation is
  slow and currently constitutes a bottleneck. One already implemented
  solution is to carry out the estimation in multiple
  processors. Additionally, we are also investigating to improve the
  efficiency of the estimation algorithm.

- Log-linear model weight adjustment is carried out by means of the
  downhill simplex algorithm, which is very slow. Downhill simplex will
  be replaced by a more efficient technique.

- Non-monotonic translation is not yet sufficiently tested, specially
  with complex corpora such as Europarl.


Support
-------
Project documentation is being developed. Such documentation include:

- This README file
- The Thot manual ("thot_manual.pdf" under the "doc" directory)

If you need additional help, you can:

- use the [github issue tracker] [8]
- [send an e-mail to the author] [9]
- join the [CasMaCat support group] [10]

Additional information about the theoretical foundations of Thot can be
found in [Ortiz-Martínez, 2011]. One interesting feature of Thot,
incremental (or online) estimation of statistical models, is also
described in [Ortiz-Martínez et al., 2010].


Citation
--------
You are welcome to use the code under the terms of the license for
research or commercial purposes, however please acknowledge its use with
a citation:

Daniel Ortiz-Martínez, Francisco Casacuberta. 
*"The New Thot Toolkit for Fully Automatic and Interactive Statistical Machine Translation"*.
14th Annual Meeting of the European Association for Computational Linguistics: System Demonstrations,
Gothenburg, Sweden, April 2014.

Here is a BiBTeX entry:

<pre>
@InProceedings{Ortiz2014,
  author    = {D.~Ortiz-Mart\'{\i}nez and F.~Casacuberta},
  title     = {The New Thot Toolkit for Fully Automatic and Interactive Statistical Machine Translation},
  booktitle = {14th Annual Meeting of the European Association for Computational Linguistics: System Demonstrations},
  year      = {2014},
  month     = {April},
  address   = {Gothenburg, Sweden},
  pages     = "45--48",
}
</pre>


Literature
----------
Daniel Ortiz-Martínez,
"*Advances in Fully-Automatic and Interactive Phrase-Based Statistical Machine Translation*". 
PhD Thesis. Universidad Politécnica de Valencia. 2011. 
Advisors: Ismael García Varea and Francisco Casacuberta. [Download] [11].

Daniel Ortiz-Martínez, Ismael García-Varea, Francisco Casacuberta. 
"*Online Learning for Interactive Statistical Machine Translation*". 
Proceedings of the North American Chapter of the Association for Computational Linguistics - 
Human Language Technologies (NAACL HLT), 2010. pp. 546-554. [Download] [12].

Daniel Ortiz-Martínez, Ismael García-Varea, Francisco Casacuberta. 
"*Phrase-level alignment generation using a smoothed loglinear phrase-based statistical alignment model*". 
Proceedings of European Association for Machine Translation (EAMT), 2008. pp. 160-169. *Best paper award*. [Download] [13]

Daniel Ortiz-Martínez, Ismael García-Varea, Francisco Casacuberta. 
"*Thot: a toolkit to train phrase-based models for statistical machine translation*".
Proc. of the Tenth Machine Translation Summit (MT-Summit),
Phuket, Thailand, September 2005. [Download] [14]


Acknowledgements
----------------
Thot is currently supported by the European Union under the [CasMaCat
research project] [5]. Thot has also received support from the Spanish
Government in a number of research projects, such as the [MIPRCV
project] [15] that belongs to the [CONSOLIDER programme] [16].


Last updated: 17 September 2014

[1]: https://www.prhlt.upv.es/page/member?user=dortiz
[2]: https://www.prhlt.upv.es/
[3]: http://www.upv.es/
[4]: http://www.casmacat.eu/index.php?n=Workbench.Workbench
[5]: http://www.casmacat.eu/
[6]: http://www.gnu.org/copyleft/lgpl.html
[7]: https://github.com/daormar/thot/archive/master.zip
[8]: https://github.com/daormar/thot/issues
[9]: mailto:daormar2@gmail.com
[10]: http://groups.google.com/group/casmacat-support/boxsubscribe
[11]: https://www.prhlt.upv.es/aigaion2/attachments/dortiz_thesis_2011.pdf-d12d165f9a2b01b0697000ed7c08c4bc.pdf
[12]: http://aclweb.org/anthology-new/N/N10/N10-1079.pdf
[13]: http://mt-archive.info/EAMT-2008-Ortiz-Martinez.pdf
[14]: http://www.mt-archive.info/MTS-2005-Ortiz-Martinez.pdf
[15]: http://miprcv.iti.upv.es/
[16]: http://www.ingenio2010.es/
