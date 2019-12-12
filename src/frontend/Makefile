MAINFILE="httpServer.cpp"                                                       
EXECUTABLE="server.exe"                                                         
PORT=8000

SRC_PATH = ../.
INCLUDES = -I $(SRC_PATH)/utils/ -I $(SRC_PATH)/parser/ -I $(SRC_PATH)/indexer/ -I $(SRC_PATH)/ranker/ -I $(TEST_PATH) $(LDFLAGS) $(CPPFLAGS)
make:                                                                           
	g++ --std=c++11 -pthread $(INCLUDES) $(MAINFILE) -o $(EXECUTABLE) -v
                                                                                
run: clean make                                                                 
	./$(EXECUTABLE) $(PORT)                                                     
                                                                                
clean:                                                                          
	rm $(EXECUTABLE)
