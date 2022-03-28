## INFO ##


First we need to clone.

    git clone https://github.com/eddic/fastcgipp.git fastcgi++

Then we make a build directory.

    mkdir fastcgi++.build
    cd fastcgi++.build

Now we need run cmake. Note that the install prefix is being explicitly set to
/usr because most server spawned FastCGI applications lack the /usr/local
library directories in the search path.

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=RELEASE ../fastcgi++

And if we want to build the PostgreSQL stuff we need to actually make it do so.

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=RELEASE -DSQL=true ../fastcgi++

Note that that was to do a release build. That means heavily optimized and not
good for debugging. If you want to do some debugging to either fastcgi++ or an
application you are developing that uses fastcgi++, do a debug build.

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=DEBUG ../fastcgi++

Or if you want some really hardcore debug and diagnostics info

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=DEBUG -D LOG_LEVEL:INT=4 ../fastcgi++

Now let's build the library itself.

    make

Then we can build the documentation if we so desire.

    make doc

Now let's install it all (doc included if it was built).

    sudo make install

Maybe we should build the unit tests?

    make tests

And of course we should run them as well.

    make test

And hey, let's build the examples too!

    make examples
