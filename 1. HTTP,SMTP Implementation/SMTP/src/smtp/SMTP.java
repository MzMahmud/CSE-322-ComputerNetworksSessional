package smtp;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import static java.lang.System.exit;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Base64;
import java.util.Scanner;

/**
 *
 * @author Jahin
 */
public class SMTP {

    public static void main(String[] args) throws UnknownHostException, IOException {
        BufferedReader br;
        Scanner sc = new Scanner(System.in);
        String mailServer = "smtp.sendgrid.net";
        
        System.out.println("Connecting to " + mailServer);
        InetAddress mailHost = InetAddress.getByName(mailServer);
        InetAddress localHost = InetAddress.getLocalHost();
        
        Socket smtpSocket = new Socket();
        smtpSocket.connect(new InetSocketAddress(mailHost, 587),20000);
        
        BufferedReader in =  new BufferedReader(new InputStreamReader(smtpSocket.getInputStream()));
        PrintWriter pr = new PrintWriter(smtpSocket.getOutputStream(),true);
        
        String initialID = in.readLine();
        System.out.println("S: " + initialID);
        pr.println("HELO "+localHost.getHostName());
        System.out.println("C: HELO "+localHost.getHostName());
        pr.flush();
        
        String welcome = in.readLine();
        System.out.println("S: " + welcome);
        
        if(welcome.charAt(0) == '5' || welcome.charAt(0) == '4'){
            System.out.println("Errorn Connecting");
            exit(1);
        }
        // TODO code application logic here
        
        //Authentication
        pr.println("AUTH LOGIN\n" +
                    "SmFoaW45OQ==\n" +
                    "amFoaW4xMjM0");
        
        System.out.println("C: AUTH LOGIN\n" +
            "SmFoaW45OQ==\n" +
            "amFoaW4xMjM0");

        
        pr.flush();
        
        welcome = in.readLine();
        System.out.println("S: " + welcome);
        
        welcome = in.readLine();
        System.out.println("S: " + welcome);
        
        welcome = in.readLine();
        System.out.println("S: " + welcome);
        
        if(welcome.charAt(0) == '5' || welcome.charAt(0) == '4'){
            System.out.println("Errorn Authenticating");
            exit(1);
        }
        
        br = new BufferedReader(new FileReader("setup.txt"));
        
        String st = br.readLine();

        pr.println("MAIL FROM: <" + st + ">");
        System.out.println("C: MAIL FROM: <" + st + ">");
        pr.flush();
        
        welcome = in.readLine();
        System.out.println("S: " + welcome);
        
        if(welcome.charAt(0) == '5' || welcome.charAt(0) == '4'){
            System.out.println("Errorn in sender mail address");
            exit(1);
        }
        
        int n = Integer.parseInt(br.readLine());
        String[] rcpt = new String[100];
        for(int i = 0; i < n; i++){
            rcpt[i] = br.readLine();
            pr.println("RCPT TO: <" + rcpt[i] + ">");
            System.out.println("C: RCPT TO: <" + rcpt[i] + ">");
            
            pr.flush();
            
            welcome = in.readLine();
            System.out.println("S: " + welcome);
            
            if(welcome.charAt(0) == '5' || welcome.charAt(0) == '4'){
                System.out.println("Errorn in reciever mail address");
                exit(1);
            }
        }
        
        pr.println("DATA");
        System.out.println("DATA");
        pr.flush();
        
        welcome = in.readLine();
        System.out.println("S: " + welcome);
        if(welcome.charAt(0) == '5' || welcome.charAt(0) == '4'){
            System.out.println("Refused to read data");
            exit(1);
        }
        
        //Mail data starts here
        String line;
        br = new BufferedReader(new FileReader("mail1.txt"));
        String mail = "From: <"+st+">\nTo: ";
        for(int i = 0; i < n; i++){
            mail+="<" + rcpt[i] + "> ";
        }
        System.out.print("Subject: ");
        mail += "\nSubject: " + br.readLine();
        mail += "\nMIME-Version: 1.0\n" +
                "Content-Type: multipart/mixed; boundary=sep\n" +
                "\n" +
                "--sep\n" +
                "Content-type: text/plain; charset=us-ascii \n\n\n";
        
        line =  br.readLine();
        while(!line.equals(".")){
            mail +=line+"\n";
            line = br.readLine();
        }
        
        //Attachment is attached if "y"
        if(br.readLine().equalsIgnoreCase("y")){
            String fname = br.readLine();
            mail += "\n--sep\n" +
                "Content--Type: application/octet-stream; name=\\\""+fname+"\\\"\n" +
                "Content-Disposition: attachment; filename=\\\""+fname+"\\\"\n" +
                "Content-Transfer-Encoding: base64\n\n\n";
            
            File f = new File(fname);		//change path of image according to you
            FileInputStream fis = new FileInputStream(f);
            byte byteArray[] = new byte[(int)f.length()];
            fis.read(byteArray);
            String imageString = Base64.getEncoder().encodeToString(byteArray);
            mail += imageString + "\n...\n";
        }
        
        mail += "--sep--\n" +
                ".\n" +
                "QUIT";
        System.out.println(mail);
        pr.println(mail);
        pr.flush();
        welcome = in.readLine();
        System.out.println(welcome);
        if(welcome.charAt(0) == '5' || welcome.charAt(0) == '4'){
            System.out.println("Could not send mail");
            exit(1);
        }
    }
    
}
