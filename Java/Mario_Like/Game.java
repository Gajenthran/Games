import javafx.application.Application;
import javafx.application.Platform;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.Group;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseButton;
import java.util.ArrayList;
import javafx.util.Duration;
import javafx.animation.Timeline;
import javafx.animation.KeyFrame;

public class Game extends Application {
	Player player = new Player(Ressources.SZ, 3 * Ressources.SZ, 4, Ressources.SZ, Ressources.SZ, 16, 16, 0, 3, false, false);
	ArrayList<Enemy> enemy = new ArrayList<Enemy>();
	Level lvl;
	int curLvl = 0;

	public void initGame() {
		lvl = new Level();
		lvl.loadLevel(curLvl);
		player.setX(1 * Ressources.SZ);
		player.setY(3 * Ressources.SZ);
		player.setValueWin(false);
		player.setImage("files/character.png");
		for(int i = 0; i < lvl.bg.size(); i++)
			switch(lvl.bg.get(i).typecell) {
				case 10:
					enemy.add(new Enemy(i%lvl.W * Ressources.SZ, i/lvl.W * Ressources.SZ, 0, Ressources.SZ, Ressources.SZ, 18, 18, 1));
					break;
				case 11:
					enemy.add(new Enemy(i%lvl.W * Ressources.SZ, i/lvl.W * Ressources.SZ, 0, Ressources.SZ, Ressources.SZ, 18, 18, 0));
					break;
				case 12:
					enemy.add(new Enemy(i%lvl.W * Ressources.SZ, i/lvl.W * Ressources.SZ, 1, Ressources.SZ, Ressources.SZ, 18, 18, 2));
					break;
			}

		for(int ent_id = 0; ent_id < enemy.size(); ent_id++)
			enemy.get(ent_id).setImage("files/enemy.png");
		curLvl++;
	}

	public static void main(String[] args) {
		launch(args);
	}

	@Override
	public void start(Stage primaryStage) {
		primaryStage.setTitle("Mario_Like");
		Group root = new Group();
		Scene scene = new Scene(root);
		primaryStage.setScene(scene);
		Canvas canvas = new Canvas(Level.screenW * Ressources.SZ, Level.screenH * Ressources.SZ);
		root.getChildren().add(canvas);
		GraphicsContext gc = canvas.getGraphicsContext2D();

		ArrayList<String> input = new ArrayList<String>();
		ArrayList<Double> mouse = new ArrayList<Double>();

		scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
			public void handle(KeyEvent event) {
				if (!input.contains(event.getCode().toString()))
					input.add(event.getCode().toString());
			}
		});

		scene.setOnKeyReleased(new EventHandler<KeyEvent>() {
			public void handle(KeyEvent event) {
				input.remove(event.getCode().toString());
			}
		});

		scene.addEventFilter(MouseEvent.MOUSE_PRESSED, new EventHandler<MouseEvent>() {
			@Override
			public void handle(MouseEvent event) {
				if(event.getButton().equals(MouseButton.PRIMARY)) {
					if(event.getClickCount() == 2) {
		      				mouse.add(event.getX());
							mouse.add(event.getY());		
					}
				}
			}
		});

		scene.setOnMouseReleased(event -> {
			mouse.clear();
		});
		
		initGame();
		Timeline gameLoop = new Timeline();
		gameLoop.setCycleCount(Timeline.INDEFINITE);
		final long timeStart = System.currentTimeMillis();
        
		KeyFrame kf = new KeyFrame(
			Duration.seconds(0.12),
			new EventHandler<ActionEvent>() {
				public void handle(ActionEvent event) {
					gc.clearRect(0, 0, Level.screenW * Ressources.SZ, Level.screenH * Ressources.SZ);
	            	if (player.getLife() <= 0 || curLvl > Level.NLevel)
	            		Platform.exit();

					if (input.contains("LEFT") || input.contains("A"))
						player.move(player.LEFT);
					else if (input.contains("UP") || input.contains("W"))
						player.move(player.UP);
					else if (input.contains("RIGHT") || input.contains("D"))
						player.move(player.RIGHT);
					else if (input.contains("DOWN") || input.contains("Z"))
						player.move(player.DOWN);
					else
						player.move(player.NOTHING);

	            	for(int ent_id = 0; ent_id < enemy.size(); ent_id++) {
						enemy.get(ent_id).move(player);
						enemy.get(ent_id).respawn(player);
						if (input.contains("S") && input.size() == 1)
							player.attack(enemy.get(ent_id));
					}


					if (input.contains("F"))
						player.quitZone();

					if (mouse.size() == 2) {
						player.position(mouse.get(0), mouse.get(1));
						mouse.clear();
					}

					player.gravity();
					lvl.draw(gc, player);
	            	player.draw(gc);
	            	for(int ent_id = 0; ent_id < enemy.size(); ent_id++)
	            		enemy.get(ent_id).draw(gc, (int)player.getX());

	            	if (player.getValueWin() == true) {
	            		lvl.clearLevel();
	            		enemy.clear();
	            		initGame();
	            	}
				}
			});
        
		gameLoop.getKeyFrames().add(kf);
		gameLoop.play();

		primaryStage.show();
	}
}