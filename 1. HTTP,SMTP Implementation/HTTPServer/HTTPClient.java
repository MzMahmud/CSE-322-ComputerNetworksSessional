import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Date;

public class HTTPClient implements Runnable{
    private Socket socket;
    private BufferedReader in;
    private OutputStream out;
    private int ID;
    private String req,file = "";
    private String []data;
    
    public HTTPClient(Socket socket,int id) {
        this.socket = socket;
        this.ID = id;
        
        try{
                
            this.in  = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));
            this.out = this.socket.getOutputStream();
                
        }catch(Exception e){
                System.out.println("Sorry. Cannot manage client [" + id + "] properly.");
                
                HTTPServer.log.println(new Date() + "\r\nSorry. Cannot manage client [" + id + "] properly.\r\n");
                HTTPServer.log.flush();
        }
    }
    
    
    @Override
    public void run() {
        try {
            
            getInput();
            sendResponse();
            
            System.out.println(new Date() + "\r\nClosing Request[" + ID + "]\r\n");
            HTTPServer.log.println(new Date() + "\r\nClosing Request[" + ID + "]\r\n");
            
            in.close();
            out.close();
        } catch (IOException ex) {
            System.out.println(new Date() + "\r\nError Sending/Reciving Data for Request[" + ID + "]\r\n");
            HTTPServer.log.println(new Date() + "\r\nError Sending/Reciving Data for Request[" + ID + "]\r\n");
        }
    }
    
    private synchronized void getInput() throws IOException{
        System.out.println(new Date()+ "\r\nInput From Request[" + ID + "]\r\n");
        HTTPServer.log.println(new Date()+ "\r\nInput From Request[" + ID + "]\r\n");
        HTTPServer.log.flush();
        
        String line = in.readLine();//The HTTP Request Line
        if(line == null) return;
        
        
        //request line GET/POST \<file> HTTP1.1
        String [] reqLine = line.split(" "); 
        req = reqLine[0];//request type = first element
        
        file = "." + reqLine[1];//file = line 2nd element
        if(file.length() <= 2)//if ".\" -> the DEFAULT FILE must be fetched
            file += HTTPServer.DEFAULT;
        
        int contentLength = 10;
        
        while(line != null){
            if(line.equals("")){//Indicates End of Header
                if("GET".equals(req)){//no moore input neeeded for get request
                    System.out.println("Terminate");
                    HTTPServer.log.println("\r\n");
                    HTTPServer.log.flush();
                    return;
                }
                
                //gets the Entity Body
                char mess[] = new char[contentLength+100];
                in.read(mess);
                String toPost = new String(mess).trim(); 
                data = toPost.split("&");
                //HTTP POST seperates form fileds line this
                //<field name1>=data&<field name2>=data
                
                for(String a:data){
                    System.out.println(a);
                }
                
                System.out.println(toPost + "\r\n");
                HTTPServer.log.println(toPost + "\r\n");
                HTTPServer.log.flush();
                
                return;
            }
            
            System.out.println(line);
            HTTPServer.log.println(line);
            
            line = in.readLine();
            if(line.contains("Content-Length:")){//gets the content length
                contentLength = new Integer(line.split("Content-Length: ")[1]);
                System.out.println("Content-Length = " + contentLength);
            }
        }
    }
    private void sendResponse() throws IOException{
        System.out.println("***MY FILE***\n" + file);
        if(file == null) return;
        File toSend = new File(file);
        if(toSend == null) return;
        
        if(!toSend.exists()){
            File E404 = new File("404.html");
                       
            sendBasicHeader(E404,true);
            
            out.write("Content-Type: text/html\r\n".getBytes()); // The type of data 
            out.write("\r\n".getBytes()); // End of headers
            
            sendText(HTTPServer.NotFOUND);//send not found.html
            return;
        }
        
        //sned header
        sendBasicHeader(toSend,false);//the file toSend,error = false
       
        if(file.matches(".+\\.html")){
            out.write("Content-Type: text/html\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: text/html\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendText(file);
            
        }else if(file.matches(".+\\.txt")){
            out.write("Content-Type: text/plain\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: text/plain\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendText(file);
            
        }else if(file.matches(".+\\.jpg")){
            out.write("Content-Type: image/jpg\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: image/jpg\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendBinary(file);
            
        }else if(file.matches(".+\\.png")){
            out.write("Content-Type: image/png\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: image/png\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendBinary(file);
            
        }else if(file.matches(".+\\.gif")){
            out.write("Content-Type: image/gif\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: image/gif\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendBinary(file);
            
        }else if(file.matches(".+\\.bmp")){
            out.write("Content-Type: image/bmp\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: image/bmp\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendBinary(file);
            
        }else if(file.matches(".+\\.pdf")){
            out.write("Content-Type: application/pdf\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: application/pdf\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendBinary(file);
            
        }else if(file.matches(".+\\.mp3")){
            out.write("Content-Type: audio/mpeg\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: audio/mpeg\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendBinary(file);
            
        }else{
            out.write("Content-Type: text/html\r\n".getBytes()); // The type of data
            HTTPServer.log.print("Content-Type: text/html\r\n");
            
            out.write("\r\n".getBytes()); // End of headers
            
            sendText("415.html");//415 Unsupported Media Type
        }
    }
    
    private void sendText(String fileName) throws IOException{
        System.out.println(new Date()+ "\r\nStart Sending file -> " + fileName + "\r\n");
        HTTPServer.log.println(new Date()+ "\r\nStart Sending file -> " + fileName + "\r\n");
        HTTPServer.log.flush();
        
        BufferedReader br = null;
        try{
            br = new BufferedReader(new FileReader(fileName)); 
            String line =  br.readLine();
            while(line != null){
                line += "\r\n";
                line = line.replace("<!REQ>", req+"</br>");
                
                
                if("POST".equals(req)){
                    String toPost = "<h3> ";
                    for(String a:data){
                        toPost += a + " </br>"; 
                    }
                    toPost += " </h3>";
                    System.out.println(toPost);
                    line = line.replace("<!POST>", toPost);
                }
                
                out.write(line.getBytes());
                
                line = br.readLine();
            }
            br.close();
        } catch (IOException e) {
            System.out.println(new Date()+ "\r\nFile-> " + fileName + " sending FAILED!\r\n");
            HTTPServer.log.println(new Date()+ "\r\nFile-> " + fileName + " sent FAILED!\r\n");
        }
        
        System.out.println(new Date()+ "\r\nFile-> " + fileName + " sent successfully!\r\n");
        HTTPServer.log.println(new Date()+ "\r\nFile-> " + fileName + " sent successfully!\r\n");
    }
    
    private void sendBinary(String fileName) throws IOException{
        System.out.println(new Date()+ "\r\nStart Sending file -> " + fileName + "\r\n");
        HTTPServer.log.println(new Date()+ "\r\nStart Sending file -> " + fileName + "\r\n");
        HTTPServer.log.flush();
        
        File file = new File(fileName);
        FileInputStream fis = new FileInputStream(file);
        BufferedInputStream bis = new BufferedInputStream(fis);
        byte[] contents;
        long fileLength = file.length();

        long current = 0;

        while(current!=fileLength){ 
                int size = 10000;
                if(fileLength - current >= size)
                        current += size;    
                else{ 
                        size = (int)(fileLength - current); 
                        current = fileLength;
                } 
                contents = new byte[size];

                bis.read(contents, 0, size); 
                out.write(contents);
        }   
        out.flush(); 
        
        System.out.println(new Date()+ "\r\nFile-> " + fileName + " sent successfully!\r\n");
        HTTPServer.log.println(new Date()+ "\r\nFile-> " + fileName + " sent successfully!\r\n");
    }

    private void sendBasicHeader(File fileName,boolean error) throws IOException {
        
        System.out.println(new Date() +"\r\nServer Response to Request[" + ID + "]\r\n");
        HTTPServer.log.println(new Date() +"\r\nServer Response to Request[" + ID + "]\r\n");
        
        // Version & status code
        if(!error){
            out.write("HTTP/1.1 200 OK\r\n".getBytes());
            HTTPServer.log.print("HTTP/1.1 200 OK\r\n");
        }else{
            out.write("HTTP/1.1 404 Not found\r\n".getBytes());
            HTTPServer.log.print("HTTP/1.1 404 Not found\r\n");
        }
        
        //Date
        String a = "Date: " + new Date().toString() + "\r\n";
        out.write(a.getBytes());
        HTTPServer.log.print(a);

        //Server
        a = "Server: " + InetAddress.getLocalHost().getHostAddress()+ ":" + HTTPServer.PORT + "\r\n";
        out.write(a.getBytes());
        HTTPServer.log.print(a);
        
        //Last Modified
        a = "Last-Modified: " + new Date(fileName.lastModified()).toString() + "\r\n";
        out.write(a.getBytes());
        HTTPServer.log.print(a);
        
        //Accept-Ranges: bytes
        out.write("Accept-Ranges: bytes\r\n".getBytes());
        HTTPServer.log.print("Accept-Ranges: bytes\r\n");
        
        //Content-Length:
        /*if("GET".equals(req)){
            a = "Content-Length: " + fileName.length() + "\r\n";
            out.write(a.getBytes());
            HTTPServer.log.print(a);
        }*/
        
        //Connection type
        out.write("Connection: close\r\n".getBytes()); // Will close stream
        HTTPServer.log.print("Connection: close\r\n");
    }
}
