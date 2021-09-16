import java.io.*;
import java.net.*;
import javax.swing.*;
import java.awt.*;

public class Client extends JFrame {

	public Client() {

		JPanel textzone = new JPanel();

		try{
            MulticastSocket mso=new MulticastSocket(2525);
            mso.joinGroup(InetAddress.getByName("225.10.20.32"));
            byte[]data=new byte[161];
			DatagramPacket paquet=new DatagramPacket(data,data.length);
		
	    	this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			this.setVisible(true);
			this.setResizable(false);
	    	this.setSize(900, 500);
			this.setTitle("Messages reçus par 225.10.20.32");
			
			// TEXTAREA ICI //
			JTextArea textZone = new JTextArea(30,80);

			textZone.setText("");
	    	textZone.setLineWrap(true);
			textZone.setWrapStyleWord(true);

			JScrollPane sp = new JScrollPane(textZone,JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
			
			this.add(sp);

			this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    	this.setVisible(true);
	    	this.pack();
			this.setTitle("Messages reçus par 225.10.20.32");
		    
            while(true){

                mso.receive(paquet);
                String st=new String(paquet.getData(),0,paquet.getLength());
				textZone.append(st + "\n");
				//sp.setPreferredSize(new Dimension(500, 500));
				//sp.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
				sp.revalidate();
				sp.repaint();
				this.add(sp);
				this.repaint();
				this.revalidate();
		

				}
						
        	} catch(Exception e){
            	e.printStackTrace();
        	}


	}




    public  static void main(String[] args){
		
		
		new Client();


	}
	







}
