/**
 * Print classes which manages all messages sent by the virtual file system
 * to the command line to be read by the user.
 * 
 * @author c3chownl, c4paprak, c3tsafat, g3dakouz 
 * 
 */
package driver;

import java.util.HashMap;

public class Print {

	/**
	 * prints out the directory contents
	 * 
	 * @param path
	 *            name
	 * @return
	 */
	public static String printDirContents(String path) {
		FileSystem[] pathPair = Input.parsePath(path);
		String pathToPrint = "";
		if (pathPair[1] instanceof File) {
			pathToPrint += pathPair[1].getName();
		} else if (pathPair[1] instanceof Directory) {
			pathToPrint += pathPair[1].getName() + ": ";
			for (Directory dir : ((Directory) pathPair[1]).getChildren()) {
				pathToPrint += dir.getName() + " ";
			}
			for (File file : ((Directory) pathPair[1]).getFiles()) {
				pathToPrint += file.getName() + " ";
			}
			pathToPrint += "\n";
		} else if (pathPair[0] instanceof Directory) {
			pathToPrint += pathPair[0].getName() + ": ";
			for (Directory dir : ((Directory) pathPair[0]).getChildren()) {
				pathToPrint += dir.getName() + " ";
			}
			for (File file : ((Directory) pathPair[0]).getFiles()) {
				pathToPrint += file.getName() + " ";
			}
			pathToPrint += "\n";
		}
		return pathToPrint;
	}

	/**
	 * Prints instructions for commands and how they behave.
	 * 
	 * @param command
	 *            the command which the user wants the manual for
	 * @return
	 */
	public static String man(String command) {
		HashMap<String, String> commandMan = new HashMap<String, String>();
		commandMan.put("cat",
				" cat [OPTION]... [FILE]...\n"
						+ "DESCRIPTION: Displays the contents  of  "
						+ "FILE  in  the  shell.");
commandMan.put("cd", "cd dirName\n" + "DESCRIPTION: Changes Directory");
commandMan.put("cp",
				"cp OLDPATH  NEWPATH\n"
						+ " DESCRIPTION: Copies file or directories in"
						+ " OLDPATH to NEWPATH,");
commandMan.put("echo",
				"echo STRING  [>  OUTFILE] or echo STRING  >>  "
						+ "OUTFILE\n "
						+ "DESCRIPTION: If OUTFILE is not"
						+ " provided, prints STRING on the "
						+ "shell. Otherwise,"
						+ "echoputs STRING into file OUTFILE. "
						+ "This creates a new file"
						+ "if OUTFILE does not exists and erases the old "
						+ "contents if "
						+ "OUTFILE already exists. If >> is in the "
						+ "argument append instead "
						+ "of overwrite.");
commandMan.put("exit", "exit\n"
		+ "DESCRIPTION: This will exit the program");
commandMan.put("ls",
				" ls [-R][PATH...]\n"
						+ "DESCRIPTION: If no path is"
						+ " given,print the contents of of the"
						+ " current directory."
						+ "Otherwise if path is a file print it's name,"
						+ " and if path is a directory, print the "
						+ "contents of the directory."
						+ "if -R is present list all subdirectories");
commandMan.put("man", "man COMMAND\n"
		+ "DESCRIPTION:  man is the system's"
		+ "manual. It prints the documentation of COMMAND.");
commandMan
		.put("mkdir",
				"mk dir DIR....\n"
						+ "DESCRIPTION Create the"
						+ "DIRECTORY(ies),each of which may be "
						+ "relative to the current"
						+ "directory or may be a full path. if they"
						+ " do not already exist.");
commandMan.put("mv",
				" mv OLDPATH NEWPATH\n"
						+ "DESCRIPTION: Move item"
						+ "OLDPATH to NEWPATH. Both "
						+ "OLDPATH and NEWPATH may be relative to"
						+ "the current directory or may be full "
						+ "paths.");
commandMan.put("pwd", "pwd\n" + "DESCRIPTION:  Print the full filename"
		+ "of the current working directory.");
commandMan.put("get", "get URL\n"
			+ "URL is a web address. Retrieve the file at URL and add it to" +
					"the current working directory");
commandMan.put("pushd", "pushd DIR\n" +
		"Saves the current working directory onto stack and then change the " +
		"new current working directory to DIR. The pushed command saves the " +
		"old current working directory in memory so that it can be returned " +
		"to at anytime(via popd)");
commandMan.put("popd", "popd\n" + 
		"Remove the top entry from the directory stack, and cd to the new top"
		+ "directory. The popd command removes the top most directory" 
		+"onto the stack and makes it the current working directory"
		+"If there is no directory, give appropriate error message");
commandMan.put("rm", "rm[-f]PATH\n"
		+"Comfirm with the user that they want to delete PATH, and if so, " +
				"remove it from the JShell file system, If path is a" +
				"directory, recursively remove all files" + 
				"and directories in it, prompting for confirmation for"
				+ "each one, If -f is supplied, do not confirm just remove"); 	

		if (commandMan.containsKey(command)) {
			return commandMan.get(command);
		} else {
			return "No Manual entry " + command;
		}
	}

	/**
	 * Prints contents in the file.
	 * 
	 * @param fileName
	 * @return fileContents
	 */
	public static String printFileContents(File file) {
		System.out.println(file.getContents());
		return file.getContents();
	}

	/**
	 * Prints back string for echo method
	 * 
	 * @param string
	 * @return string
	 */
	public static String printEcho(String string) {
		return string;
	}

	/**
	 * returns String of working directory path
	 * 
	 * @return
	 */
	public static String printWorkingDirectory() {
		return Directory.workingDirectory.directoryLocation();
	}
	
	/**
	 * Prints the result of a grep command onto the console.
	 * 
	 * @return result of grep command.
	 */
	public static String printGrep(String result) {
		String grepResult = "Files which match given regular" +
							"expression are: \n";
		if (result.equals("")) {
			grepResult += "No items matched the given regular expression.";
		}
		else {
			grepResult += result;
		}
		return grepResult;
	}
}
