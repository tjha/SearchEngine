MAINFILE="httpServer.cpp"                                                       
EXECUTABLE="server.exe"                                                         
PORT=8000                                                                       
                                                                                
make:                                                                           
	g++ --std=c++11 -pthread $(MAINFILE) -o $(EXECUTABLE)
                                                                                
run: clean make                                                                 
	./$(EXECUTABLE) $(PORT)                                                     
                                                                                
clean:                                                                          
	rm $(EXECUTABLE)
