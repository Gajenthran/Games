import java.io.*;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import java.util.ArrayList;
import javafx.scene.paint.Color;

public class Level {
	private final int imgW = 16;
	private final int imgH = 16;
	public static final int W = 90;
	public static final int H = 12;
	public static final int screenW = 16;
	public static final int screenH = 12;
	public static final int NLevel = 3;
	public static ArrayList<Ressources> bg;
	private Image image = new Image("files/background.png");

	public Level() {
 		this.bg = new ArrayList<Ressources>();
	}

	public void loadLevel(int curLvl) {
		String line = "", level = "";
		try {
			InputStream file; 
			switch(curLvl) {
				case 0:
					file = new FileInputStream("level/level1.txt");
					break;
				case 1:
					file = new FileInputStream("level/level2.txt");
					break;	
				case 2:
					file = new FileInputStream("level/level3.txt");
					break;			
				default:
					file = new FileInputStream("level/level1.txt");
					break;
			}

			InputStreamReader read=new InputStreamReader(file);
			BufferedReader buff=new BufferedReader(read);
			while ((line=buff.readLine())!=null)
			level += line;
		
			buff.close(); 
		}	

		catch (Exception e){
			System.out.println(e.toString());
		}

		int y, x, SZ = Ressources.SZ;
		for (y = 0; y < H; y++) {
			for (x = 0; x < W; x++) {
				bg.add(new Ressources(level.charAt(y * W + x) - 'A', x * SZ, y * SZ, SZ, SZ));
			} 
		}
	}

	public void drawDetails(GraphicsContext gc, int playerLife, int playerPoint, int playerSword) {
		int NIcon = 3;
		int tabIcon[] = {10, 100, 200, 5};
		int tabFont[] = {30, 120, 220, 20};
		Font font = Font.font("Helvetica", FontWeight.BOLD, 20);
		gc.setFont(font);
		Color pink = Color.rgb(240, 183, 221);
		Color blue = Color.rgb(0, 120, 248);
		Color yellow = Color.rgb(247, 206, 70);
		Color actuelC = Color.rgb(255, 255, 255);
		String lifeTxt = "x" + playerLife;
		String scoreTxt = "x" + playerPoint;
		String durabilityTxt = "x" + playerSword;
		String txt = "";

		for (int i = 0; i < NIcon; i++) {
			gc.drawImage(image, imgW * i, imgH, imgW, imgH, tabIcon[i], tabIcon[3], 20, 20);
			gc.setFill(Color.WHITE);
			gc.setLineWidth(0.5);
			switch(i) {
				case 0:
					txt = lifeTxt;
					actuelC = pink;
					break;
				case 1:
					txt = scoreTxt;
					actuelC = yellow;
					break;
				case 2:
					txt = durabilityTxt;
					actuelC = blue;
					break;
			}
			gc.setStroke(actuelC);
			gc.fillText(txt, tabFont[i], tabFont[3]);
			gc.strokeText(txt, tabFont[i], tabFont[3]);
		}
	}

    public void draw(GraphicsContext gc, Player player) {
    	int y, x, l, c;
    	int moveX = ((int)player.getX()/Ressources.SZ) - screenW /2, diffX = 0, limitX;

    	if (moveX < 0)
    		moveX = 0;

    	limitX = moveX + screenW;
    	
    	if (limitX >= W) {
    		diffX = limitX - W;
    		limitX = W;
    	}

    	for (y = 0, l = 0; y < screenH; y++, l++) {
    		for (x = moveX - diffX, c = 0; x < limitX; x++, c++) {
    			gc.drawImage(image, bg.get(y * W + x).typecell * imgW, 0, imgW, imgH, c * Ressources.SZ, l * Ressources.SZ, Ressources.SZ, Ressources.SZ);
    		}
    	}

    	drawDetails(gc, player.getLife(), player.getPoints(), player.getSwordDurability());
    }

    public void clearLevel() {
    	bg.clear();
    }
}