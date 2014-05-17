package com.tetris.listener;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import com.tetris.controller.TetrisController;

public class TetrisKeyListener implements KeyListener {

	private TetrisController controller;
	
	public TetrisKeyListener(TetrisController controller) {
		this.controller = controller;
	}
	
	@Override
	public void keyTyped(KeyEvent e) {
		
	}

	@Override
	public void keyPressed(KeyEvent e) {
		controller.gameKeyControls(e.getKeyCode());
	}

	@Override
	public void keyReleased(KeyEvent e) {
		
	}

}
