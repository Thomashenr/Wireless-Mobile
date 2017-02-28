package wireless;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Random;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

public class UDP_Client1 extends JFrame{

	   private static DatagramSocket socket;
	   public static int c=0;
	   public static int ack=1;
	   public static String r="1";
	   public static String msg="";
	   public static String old_msg="0";
	   
	 public static void main(String[] args) throws InterruptedException, IOException {
		   UDP_Client1 client=new UDP_Client1();
		   Scanner s = new Scanner(System.in);
		   System.out.println("Enter Gremlin Function Value : ");
		   String g=s.nextLine();
		   
		   if(c!=5){
			 
          	msg="0123456";
          	int z=msg.getBytes().length;
          	if(msg.getBytes().length!=10)
          	{
      		  for(int i=10;i>z;i--)
      		  {
      			  msg=msg+"$";
      		  }
      	  	 }
          	old_msg=msg;
          System.out.println("\nSending message packet: "+msg);
         //for Gremlin
		 if(!g.equals("0"))
		 {
	          StringBuilder sb=new StringBuilder(msg);
	          int N = msg.length();
	          Random r =new Random();
	          int ig=Integer.parseInt(g);
	          for(int i=0;i<ig;i++)
	          {
	        	  N = msg.length();
	        	  sb.deleteCharAt(r.nextInt(N));
	          }
	          msg=sb.toString();

		 }
          
          client.sendPacket();
		
		  
		    r=client.readyToReceivPacket();
		  System.out.println("received message from server :"+r);
		  
		  while(r.trim().equals("0"))
		  {
			  System.out.println("Sending again");
			  byte buff[]=old_msg.getBytes();
	            System.out.println("client side bytes :"+buff+"    "+buff.length);
	            DatagramPacket packetSend=
	               new DatagramPacket(buff, buff.length,
	               InetAddress.getLocalHost(), 12345);
	            socket.send(packetSend);
	            System.out.println("\nPacket sent");
	            r=client.readyToReceivPacket();
	  		  System.out.println("received message from server :"+r);

		  }
		  //  TimeUnit.SECONDS.sleep(5);
		  //ack=Integer.parseInt(a);
	   }
	 }
	

   public void sendPacket() throws InterruptedException{
	   UDP_Client1 rpacket=new UDP_Client1();
	   try{
	         socket=new DatagramSocket();
	      }catch(SocketException ex){
	         System.exit(1);
	      }
	         try{
	        	
	            byte buff[]=msg.getBytes();
	            System.out.println("client side bytes :"+buff+"    "+buff.length);
	            DatagramPacket packetSend=
	               new DatagramPacket(buff, buff.length,
	               InetAddress.getLocalHost(), 12345);
	            socket.send(packetSend);
	            System.out.println("\nPacket sent");
	            c++;
	       
	           }catch(IOException ex){
	            System.out.println(ex.getMessage());
	         }
	      
   }

   public String readyToReceivPacket() throws InterruptedException{
      while(true){
         try{
        	 
        	 byte buff1[]=new byte[128];
            DatagramPacket packet=
               new DatagramPacket(buff1,buff1.length);
            socket.receive(packet);
            String r_p_server=new String(packet.getData());
           // Integer rp=new Integer(packet.getData());
            System.out.println("\nHost: " + packet.getAddress()
                    + "\nPort: " + packet.getPort()
                    + "\nLength: " + packet.getLength()
                    + "\nData: "
                    + r_p_server);           
            return r_p_server;

         }catch(IOException ex){
	            System.out.println(ex.getMessage());

         }
      }
   }

 
  

}




