package com.tetris.view;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.util.Queue;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

import com.tetris.controller.TetrisController;
import com.tetris.listener.TetrisKeyListener;
import com.tetris.model.Piece;
import com.tetris.model.TetrisBoard;

public class TetrisView extends JFrame implements Runnable {

	private static final long serialVersionUID = 1L;
	
	private static final int INTERVAL = 50; 
	
	private TetrisController controller;
	private int [][] data = new int [10][20];
	
	private BufferedImage img;
	private Queue<Piece> pieceQueue;
	
//	private static final int WIDTH = 480;
//	private static final int HEIGHT = 960;
//	private static final int SCORE_BOARD_WIDTH = 240;
	
	private static final int WIDTH = 240;
	private static final int HEIGHT = 480;
	private static final int SCORE_BOARD_WIDTH = 120;
	
	private static final int PADDING = 2;
	
	private int boxWidth;
	
	public TetrisView() {
		this(WIDTH + SCORE_BOARD_WIDTH, HEIGHT);
	}
	
	private TetrisView(int width, int height) {
		this.setTitle("Tetris");
		this.setSize(width, height);
		this.setResizable(false);
		this.setLocationRelativeTo(null);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setAlwaysOnTop(true);
		
		img = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
	}
	
	public void start() {
		this.addKeyListener(new TetrisKeyListener(controller));
		this.pieceQueue = controller.getPieceQueue();
		
		data = controller.getBoard();
		boxWidth = this.getHeight() / data.length;
		
		this.setVisible(true);
		SwingUtilities.invokeLater(this);		// swing safe threading
	}
	
	@Override
	public void paint(Graphics g) {
		Graphics2D g2d = img.createGraphics();
		g2d.setBackground(Color.GRAY);
		g2d.clearRect(0, 0, this.getWidth(), this.getHeight());
		
		// add your scene drawing here
		drawGrid(g2d);
		drawScore(g2d);
		drawPieceQueue(g2d);
		
		// double buffering
		g.drawImage(img, 0, 0, this.getWidth(), this.getHeight(), this);
	}

	public void drawGrid(Graphics g) {
		Graphics2D g2d = (Graphics2D)g.create();
		for (int r = 0; r < data.length; r++) {
			for (int c = 0; c < data[r].length; c++) {
				// change color according to the cell state
				switch (data[r][c]) {
				case TetrisBoard.EMPTY:
					g2d.setColor(Color.DARK_GRAY);
					break;
				case TetrisBoard.FILLED:
					g2d.setColor(Color.GRAY);
					break;
				case TetrisBoard.MOVING:
					g2d.setColor(pieceQueue.peek().getColor());
					break;
				}
				g2d.fill3DRect(boxWidth * c + PADDING, boxWidth * r + PADDING, boxWidth - PADDING, boxWidth - PADDING, true);
			}
		}
		g2d.dispose();
	}
	
	public void drawScore(Graphics g) {
		Graphics2D g2d = (Graphics2D)g.create();
		g2d.setColor(Color.WHITE);
		g2d.drawString("Score:",WIDTH + 20, 20);
		g2d.dispose();
	}
	
	public void drawPieceQueue(Graphics g) {
		Graphics2D g2d = (Graphics2D)g.create();
		g2d.setColor(Color.WHITE);
		
		int offsetX = WIDTH + 20;
		int offsetY = this.getHeight() / 5;
		for (Piece piece : pieceQueue) {
			g2d.setColor(piece.getColor());
			Point [] body = piece.getBody();
			for (Point pt : body) {
				g2d.fill3DRect(offsetX +  pt.x * boxWidth + PADDING, offsetY -  pt.y * boxWidth + + PADDING, boxWidth - PADDING, boxWidth - PADDING, true);
			}
			offsetY += this.getHeight() / 5;
		}
		g2d.dispose();
	}
	
	public void setController(TetrisController controller) {
		this.controller = controller;
	}

	public void setData(int[][] data) {
		this.data = data;
	}
	
	public void setPieceQueue(Queue<Piece> pieceQueue) {
		this.pieceQueue = pieceQueue;
	}

	@Override
	public void run() {
		Timer timer = new Timer(INTERVAL, new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				// add the actions here
				repaint();		// redraw the scene
			}
		});
		timer.start();
	}	
}