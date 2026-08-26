# Blokus (C++ / SFML / GoogleTest)
Blokus game, built in C++ using the [SFML](https://github.com/SFML/SFML) library, unit-tested with [GoogleTest](https://github.com/google/googletest). 

Continuous integration using a basic GitHub action. 


## Run Blokus 
Start the Server
```
docker compose up --build 
```

Start the Client
```
./build/bin/BlokusClient
```

### Create build directory
```
mkdir build
```
### Configure
```
cd build
cmake ..
```
### Build
```
cmake --build .
```
### Run Tests
Move to project folder
```
cd ..
```
Run tests
```
./build/bin/tests/BlokusClientTests
./build/bin/tests/BlokusSharedTests
./build/bin/tests/BlokusServerTests
```

### Debug Unit Test
- Run Unit test on lldb
```
lldb ./bin/tests/BlokusClientTests
```

- Set Breakpoint Example
```
(lldb) b Player.cpp:45
```

`n` (next line)

`s` (step into function)

`p mHeldPiece` (print the value of a variable)

`c` (continue to next breakpoint)

- Backtrace Example
```
(lldb) run
(lldb) bt
```

### Visual Studio
`Ctrl+S` to configure

Click `Build` in the menu bar and click `Build All`

Run tests by choosing `BlokusClientTests` and the play button

Run the app by selecting `BlokusClient` and the play button


## Project Management
Tracked progress using Github's [Backlog](https://github.com/orgs/kyuhyunp-dev/projects/1)





