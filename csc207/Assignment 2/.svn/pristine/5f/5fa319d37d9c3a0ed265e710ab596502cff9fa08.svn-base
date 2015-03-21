/**
 * AllCommands has the appropriate methods and method calls that Input needs 
 * to use to complete assigned commands. 
 * 
 * Mainly for clearing clutter in Input and managing the current 
 * working directory.
 * 
 * @author c3chownl, c4paprak, c3tsafat, g3dakouz
 *
 */

package driver;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class AllCommands {

	/**
	 * Prints a string on the command line
	 * 
	 * @param stringToEcho the string to be printed
	 */
	public static void echo(String stringToEcho) {
		System.out.println(Print.printEcho(stringToEcho));
	}
	
	/**
	 * Prints a string to a file by either appending the string to the file or 
	 * overwriting the file completely with the new string. 
	 * 
	 * @param string the string to be printed
	 * @param fileName the name of the file that the string will be printed to
	 * @param overwrite determine if the file will lose all of its previous 
	 * contents
	 */
	public static void echoFile(String string, String fileName, 
			Boolean overwrite) {
		FileSystem file = Directory.workingDirectory.fileSystemNameToObject(
				fileName);
		if (file == null) {
			new File(fileName, Directory.workingDirectory, string);
		}
		else if (file instanceof File) {
			((File) file).alterContents(string, overwrite);
		}
	}
	
	/**
	 * Prints a message through the error stream onto the command line and 
	 * accepts 
	 * @param errorMessage
	 * @return
	 */
	public static String raiseError(String errorMessage) {
		System.err.println(errorMessage);
		return errorMessage;
	}
	
	/**
	 * Terminates the command line through the System.exit(int) method.
	 */
	public static void exitShell() {
		System.exit(0);
	}
	
	/**
	 * Changes the working directory to a new directory.
	 * 
	 * @param newDir the new working directory.
	 */
	public static void changeWorkingDirectory(Directory newDir) {
		if (newDir != null) {
			newDir.setWorkingDirectory();
			String newPath = newDir.directoryLocation();
			if (newPath.equals("/")) {
				JShell.setPrompt("");
			}
			else {
				JShell.setPrompt(newPath);
			}
		}
	}
	
	/**
	 * Recursively print directory contents.
	 * 
	 * @param path list of paths to print from
	 */
	public static String recursivePrintDirContents(String path) {
		Directory dir;
		String toPrint = "";
		if (path.equals(Directory.workingDirectory.directoryLocation())) {
			dir = Directory.workingDirectory;
		}
		else {
			dir = (Directory) Input.parsePath(path)[1];
		}
		toPrint += Print.printDirContents(path);
		for (Directory child : dir.getChildren()) {
			toPrint += recursivePrintDirContents(child.directoryLocation());
		}
		return toPrint;

	}
	/**
	 * Matches regular expressions against lines in files and returns the lines
	 * in those files which match the regular expression provided.
	 * 
	 * This operates recursively if the given object to check against is a 
	 * directory.
	 * 
	 * @param regex The regular expression to match against.
	 * @param path The path to check.
	 * @return The locations of all matched lines.
	 */
	public static String grep(String regex, FileSystem fileSystem) {
		String result = "";
		Pattern regexPattern = Pattern.compile(regex);
		if (fileSystem instanceof File) {
			Matcher m = regexPattern.matcher(((File) fileSystem).getContents());
			if (m.matches()) {
				result = result + ((File) fileSystem).fileLocation() + "\n";
			}
		}
		if (fileSystem instanceof Directory) {
			for (File childFile : ((Directory) fileSystem).getFiles()) {
				result += grep(regex, childFile);
			}
			for (Directory childDir : ((Directory) fileSystem).getChildren()) {
				result += grep(regex, childDir);
			}
		}
		return result;
	}
}
	
