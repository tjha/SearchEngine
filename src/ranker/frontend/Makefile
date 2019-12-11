MAINFILE="httpServer.cpp"                                                       
EXECUTABLE="server.exe"                                                         
PORT=8000

SRC_PATH = ../.
INCLUDES = -I $(SRC_PATH)/utils/ -I $(SRC_PATH)/parser/ -I $(SRC_PATH)/spinarak/ -I $(TEST_PATH) $(LDFLAGS) $(CPPFLAGS)
make:                                                                           
	g++ --std=c++11 -pthread -I ../../utils/  $(MAINFILE) -o $(EXECUTABLE)
                                                                                
run: clean make                                                                 
	./$(EXECUTABLE) $(PORT)                                                     
                                                                                
clean:                                                                          
	rm $(EXECUTABLE)
