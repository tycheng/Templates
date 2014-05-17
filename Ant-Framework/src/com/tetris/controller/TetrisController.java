package com.tetris.controller;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Random;

import javax.swing.Timer;

import com.tetris.model.Piece;
import com.tetris.model.TetrisBoard;
import com.tetris.model.TetrisPiece;
import com.tetris.view.TetrisView;

public class TetrisController {

	private TetrisBoard board;
	private TetrisView view;

	private Piece [] pieces;
	private Piece currentPiece;
	
	private int currentPosX;
	private int currentPosY;
	
	private int currentPosXLimit;
	private int currentPosYLimit;

	private int prevPosX;
	private int prevPosY;
	
	private Random random = new Random();
	private Queue<Piece> queue = new LinkedList<Piece>();

	private static final int WAITING_SEQUENCE_LIMIT = 5;
	private static final int INTERVAL = 500;

	private int columns;
	private int rows;

	public TetrisController() {
		pieces = TetrisPiece.getPieces();
	}
	
	// populate the piece queue and maintain it to be number of 5
	private Piece populateQueue() {
		for (int i = queue.size(); i < WAITING_SEQUENCE_LIMIT; i++) {
			queue.add(pieces[random.nextInt(pieces.length)]);
		}
		return queue.poll();
	}

	public void setBoard(TetrisBoard board) {
		this.board = board;
	}

	public void setView(TetrisView view) {
		this.view = view;
	}

	// threading method to update the game
	public void update() {
		int [][] data = board.getBoard();
		columns = data[0].length;
		rows = data.length;
		
		currentPiece = getNextPiece();

		Timer timer = new Timer(INTERVAL, new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				prevPosX = currentPosX;
				prevPosY = currentPosY;
				board.undo();
				int state = board.place(currentPiece, currentPosX, currentPosY);
				// crashing with others or reaching the bottom
				processPiece(state);
				view.setData(board.getBoard());
				currentPosY++;
			}
		});
		timer.start();
	}

	public void gameKeyControls(int keyCode) {
		prevPosX = currentPosX;
		prevPosY = currentPosY;
		
		switch (keyCode) {
		case KeyEvent.VK_UP:
			currentPiece = currentPiece.nextRotation();
			break;
		case KeyEvent.VK_LEFT:
			currentPosX--;
			break;
		case KeyEvent.VK_RIGHT:
			currentPosX++;
			break;
		case KeyEvent.VK_DOWN:
			currentPosY++;
			break;
		}
		restrictXY();
		
		// immediately refresh the screen
		board.undo();
		int state = board.place(currentPiece, currentPosX, currentPosY);
		processPiece(state);
		view.setData(board.getBoard());
	}

	private void restrictXY() {
		// restriction on x
		if (currentPosX < 0)
			currentPosX = 0;
		else if (currentPosX > currentPosXLimit)
			currentPosX = currentPosXLimit;
		// restriction on y
		if (currentPosY >= currentPosYLimit)
			currentPosY = currentPosYLimit;
		
		currentPosXLimit = getXLimit();
		currentPosYLimit = getYLimit(currentPosX, currentPosY);
	}
	
	private void processPiece(int state) {
		// reach the bottom or crash with other pieces
		if (state == TetrisBoard.PLACE_BAD ||
				state == TetrisBoard.PLACE_OUT_OF_BOUNDS) {
			board.undo();
			currentPosYLimit = getYLimit(prevPosX, prevPosY);
			System.out.println("Prev X: " + prevPosX + "\tPrev Y: " + prevPosY);
			System.out.println("currentX: " + currentPosX + "\tcurrentY: " + currentPosY);
			System.out.println("limit X: " + currentPosXLimit + "\tlimit Y: " + currentPosYLimit);
			currentPosX = prevPosX;
			currentPosY = prevPosY;
			// fixed
			if (currentPosY >= currentPosYLimit) {
				board.place(currentPiece, currentPosX, currentPosYLimit - 1);
				board.commit();
				board.clearRows();
				// reset the position
				currentPiece = getNextPiece();
				view.setPieceQueue(queue);
			}
			else {
				board.place(currentPiece, prevPosX, prevPosY);
			}
		}
	} 
	
	private Piece getNextPiece() {
		currentPiece = populateQueue();
		currentPosXLimit = getXLimit();
		currentPosX = random.nextInt(currentPosXLimit);
		currentPosY = 0;
		currentPosYLimit = getYLimit(currentPosX, currentPosY);
		return currentPiece;
	}
	
	private int getXLimit() {
		return columns - currentPiece.getWidth();
	}
	
	private int getYLimit(int currentPosX, int currentPosY) {
		int y = currentPosY;
		int state;
		boolean stop = false;
		do {
			board.undo();
			state = board.place(currentPiece, currentPosX, y++);
			stop = (state == TetrisBoard.PLACE_BAD) || 
					(state == TetrisBoard.PLACE_OUT_OF_BOUNDS && y >= rows);
		} while(!stop);
		board.undo();
		return Math.min(rows, y - 1);
	}
	
	public Queue<Piece> getPieceQueue() {
		return queue;
	}
	
	public int [][] getBoard() {
		return board.getBoard();
	}
}