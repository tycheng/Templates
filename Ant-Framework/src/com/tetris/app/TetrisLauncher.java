package com.tetris.app;

import com.tetris.controller.TetrisController;
import com.tetris.model.TetrisBoard;
import com.tetris.view.TetrisView;

public class TetrisLauncher {

	public static void main(String[] args) {
		TetrisView view = new TetrisView();
		TetrisController controller = new TetrisController();
		TetrisBoard board = new TetrisBoard(20,10);
		
		view.setController(controller);
		
		controller.setView(view);
		controller.setBoard(board);
		
		controller.update();
		view.start();
	}

}