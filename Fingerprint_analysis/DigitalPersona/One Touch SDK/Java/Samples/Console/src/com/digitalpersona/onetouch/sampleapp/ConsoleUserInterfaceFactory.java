package com.digitalpersona.onetouch.sampleapp;

import com.digitalpersona.onetouch.*;
import com.digitalpersona.onetouch.capture.DPFPCapture;
import com.digitalpersona.onetouch.capture.DPFPCapturePriority;
import com.digitalpersona.onetouch.capture.event.DPFPDataEvent;
import com.digitalpersona.onetouch.capture.event.DPFPDataListener;
import com.digitalpersona.onetouch.capture.event.DPFPReaderStatusAdapter;
import com.digitalpersona.onetouch.capture.event.DPFPReaderStatusEvent;
import com.digitalpersona.onetouch.processing.DPFPEnrollment;
import com.digitalpersona.onetouch.processing.DPFPFeatureExtraction;
import com.digitalpersona.onetouch.processing.DPFPImageQualityException;
import com.digitalpersona.onetouch.readers.DPFPReaderDescription;
import com.digitalpersona.onetouch.readers.DPFPReadersCollection;
import com.digitalpersona.onetouch.verification.DPFPVerification;
import com.digitalpersona.onetouch.verification.DPFPVerificationResult;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.EnumMap;
import java.util.Vector;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Implementation of UserInterface.Factory interface that creates a console-based user interface
 */
public class ConsoleUserInterfaceFactory implements UserInterface.Factory {

    /**
     * Creates an object implementing UserInterface interface
     *
     * @param userDatabase user database to be used with the ui
     * @return created instance
     */
    public UserInterface createUI(UserDatabase userDatabase) {
        return new ConsoleUserInterface(userDatabase);
    }

    /**
     * Console-based UserInterface
     */
    private static class ConsoleUserInterface 
    	implements UserInterface 
    {

        /**
         * keeps a user database
         */
        private UserDatabase userDatabase;

        /**
         * Constructs an instance
         *
         * @param userDatabase user database to be used with the ui
         */
        public ConsoleUserInterface(UserDatabase userDatabase) {
            this.userDatabase = userDatabase;
        }

        /**
         * When an object implementing interface <code>Runnable</code> is used
         * to create a thread, starting the thread causes the object's
         * <code>run</code> method to be called in that separately executing
         * thread.
         * <p/>
         * The general contract of the method <code>run</code> is that it may
         * take any action whatsoever.
         *
         * @see Thread#run()
         */
        public void run() {
            System.out.println("\n*** Console UI ***");

            String activeReader = null;
            boolean readerSelected = false;

            int res;
            while ((res = MenuShow(mainMenu, MENU_WITH_EXIT)) != exitItem.getValue()) try {
                switch (res) {
                    case MAIN_MENU_ADD:
                        addUser();
                        break;
                    case MAIN_MENU_ENROLL:
                        if (readerSelected)
                            register(activeReader);
                        else
                            System.out.println("No reader selected");
                        break;
                    case MAIN_MENU_VERIFY:
                        if (readerSelected)
                            verify(activeReader);
                        else
                            System.out.println("No reader selected");
                        break;
                    case MAIN_MENU_ENUMERATE:
                        listReaders();
                        break;
                    case MAIN_MENU_SELECT:
                        try {
                            activeReader = selectReader(activeReader);
                            readerSelected = true;
                        } catch (IndexOutOfBoundsException e) {
                            System.out.println("There are no readers available");
                        }
                        break;
                }
            } catch (Exception e) { }
        }

        /**
         * Console menu item
         */
        private static class MenuItem
        {
            private String text;
            private int value;

            /**
             * Creates a menu item
             *
             * @param text  item text
             * @param value value to return if item is chosen
             */
            public MenuItem(String text, int value) {
                this.text = text;
                this.value = value;
            }

            /**
             * Returns the menu item's text
             *
             * @return menu item text
             */
            public String getText() {
                return text;
            }

            /**
             * Returns the menu item's associated value
             *
             * @return associated value
             */
            public int getValue() {
                return value;
            }
        }

        /**
         * Specifies that menu should be appended with "Back" item
         */
        private static final int MENU_WITH_BACK = 2;

        /**
         * Specifies that menu should be appended with "Exit" item
         */
        private static final int MENU_WITH_EXIT = 1;

        /**
         * "Exit" menu item
         */
        private static final MenuItem exitItem = new MenuItem("Exit the application", -1);

        /**
         * "Back" menu item
         */
        private static final MenuItem backItem = new MenuItem("Return to the previous menu", -2);

        private static final int MAIN_MENU_ENUMERATE = 101;
        private static final int MAIN_MENU_SELECT = 102;
        private static final int MAIN_MENU_ADD = 103;
        private static final int MAIN_MENU_ENROLL = 104;
        private static final int MAIN_MENU_VERIFY = 105;

        private static final Vector<MenuItem> mainMenu;
        static {
            mainMenu = new Vector<MenuItem>();
            mainMenu.add(new MenuItem("List all available readers", MAIN_MENU_ENUMERATE));
            mainMenu.add(new MenuItem("Select a reader", MAIN_MENU_SELECT));
            mainMenu.add(new MenuItem("Add a person to the database", MAIN_MENU_ADD));
            mainMenu.add(new MenuItem("Perform fingerprint enrollment", MAIN_MENU_ENROLL));
            mainMenu.add(new MenuItem("Perform fingerprint verification", MAIN_MENU_VERIFY));
        }

        private static final EnumMap<DPFPFingerIndex, String> fingerNames;
        static {
        	fingerNames = new EnumMap<DPFPFingerIndex, String>(DPFPFingerIndex.class);
        	fingerNames.put(DPFPFingerIndex.LEFT_PINKY,	  "left pinky");
        	fingerNames.put(DPFPFingerIndex.LEFT_RING,    "left ring");
        	fingerNames.put(DPFPFingerIndex.LEFT_MIDDLE,  "left middle");
        	fingerNames.put(DPFPFingerIndex.LEFT_INDEX,   "left index");
        	fingerNames.put(DPFPFingerIndex.LEFT_THUMB,   "left thumb");
        	fingerNames.put(DPFPFingerIndex.RIGHT_PINKY,  "right pinky");
        	fingerNames.put(DPFPFingerIndex.RIGHT_RING,   "right ring");
        	fingerNames.put(DPFPFingerIndex.RIGHT_MIDDLE, "right middle");
        	fingerNames.put(DPFPFingerIndex.RIGHT_INDEX,  "right index");
        	fingerNames.put(DPFPFingerIndex.RIGHT_THUMB,  "right thumb");
        }
        
        private int MenuShow(Vector<MenuItem> menu, int nMenuFlags) {
            int choice = 0;

            if (menu == null)
                return choice;

            while (true) {
                System.out.println();
                for (int i = 0; i < menu.size(); ++i)
                    System.out.printf("%d: %s\n", i + 1, menu.elementAt(i).getText());

                StringBuilder sb = new StringBuilder();
                sb.append(String.format("Choose an option (1 - %d", menu.size()));
                if ((nMenuFlags & MENU_WITH_BACK) != 0) {
                    System.out.printf("\nR: %s\n\n", backItem.getText());
                    sb.append(", R");
                }
                if ((nMenuFlags & MENU_WITH_EXIT) != 0) {
                    System.out.printf("\nE: %s\n\n", exitItem.getText());
                    sb.append(", E");
                }
                sb.append("): ");

                String userInput = ShowDialog(sb.toString());

                if ((nMenuFlags & MENU_WITH_EXIT) != 0 && userInput.equalsIgnoreCase("E")) {
                    choice = exitItem.getValue();
                    break;
                }
                if ((nMenuFlags & MENU_WITH_BACK) != 0 && userInput.equalsIgnoreCase("R")) {
                    choice = backItem.getValue();
                    break;
                }

                int nInput;

                try {
                    nInput = Integer.parseInt(userInput);
                } catch (NumberFormatException e) {
                    System.out.printf("\nInvalid input: \"%s\"\n", userInput);
                    continue;
                }

                if (nInput < 1 || nInput > menu.size()) {
                    System.out.printf("\nIncorrect choice: \"%s\"\n", userInput);
                    continue;
                }

                choice = menu.elementAt(nInput - 1).getValue();
                break;
            }

            System.out.println();
            return choice;
        }

        /**
         * Adds user to the database
         */
        private void addUser() {

            System.out.printf("Adding person to the database...\n");
            String username = ShowDialog("Enter a name: ");

            if (userDatabase.addUser(username) != null) {
                System.out.printf("\"%s\" was added to the database.\n", username);
            } else {
                System.out.printf("\"%s\" was not added to the database.\n", username);
            }

        }

        /**
         * register() looks for the user in the database, invokes CreateRegistrationTemplate(),
         * and stores the template in the database.
         *
         * @param activeReader reader to use for fingerprint acquisition
         */
        private void register(String activeReader) {
            System.out.printf("Performing fingerprint enrollment...\n");
            String username = ShowDialog("Enter the name of the person to be enrolled: ");

            UserDatabase.User user = userDatabase.getUser(username);
            if (user == null) {
                System.out.printf("\"%s\" was not found in the database.\n", username);
            } else {
                StringBuilder sb = new StringBuilder();
                for (DPFPFingerIndex finger : DPFPFingerIndex.values()) {
                    if (user.getTemplate(finger) != null)
                        sb.append("    ").append(fingerName(finger)).append("\n");
                }

                String fingers = sb.toString();
                if (fingers.length() > 0) {
                    System.out.printf("The following fingers for \"%s\" have already been enrolled:\n%sChoose a finger to enroll:", username, fingers);
                } else {
                    System.out.printf("No fingers for \"%s\" have been enrolled. Choose a finger to enroll:", username);
                }

                Vector<MenuItem> menu = new Vector<MenuItem>();
                for (DPFPFingerIndex finger : DPFPFingerIndex.values())
                    menu.add(new MenuItem(fingerName(finger), finger.ordinal()));

                int nFinger = MenuShow(menu, MENU_WITH_BACK);

                if (nFinger == backItem.getValue()) {
                    System.out.printf("Enrollment canceled.\n");
                } else {
                    try {
                        DPFPFingerIndex finger = DPFPFingerIndex.values()[nFinger];
                        DPFPFeatureExtraction featureExtractor = DPFPGlobal.getFeatureExtractionFactory().createFeatureExtraction();
                        DPFPEnrollment enrollment = DPFPGlobal.getEnrollmentFactory().createEnrollment();

                        while (enrollment.getFeaturesNeeded() > 0)
                        {
                            DPFPSample sample = getSample(activeReader, 
                            	String.format("Scan your %s finger (%d remaining)\n", fingerName(finger), enrollment.getFeaturesNeeded()));
                            if (sample == null)
                                continue;

                            DPFPFeatureSet featureSet;
                            try {
                                featureSet = featureExtractor.createFeatureSet(sample, DPFPDataPurpose.DATA_PURPOSE_ENROLLMENT);
                            } catch (DPFPImageQualityException e) {
                                System.out.printf("Bad image quality: \"%s\". Try again. \n", e.getCaptureFeedback().toString());
                                continue;
                            }

                            enrollment.addFeatures(featureSet);
                        }

                        DPFPTemplate template = enrollment.getTemplate();
                        user.setTemplate(finger, template);

                        System.out.printf("The %s was enrolled.\n", fingerprintName(finger));
                    } catch (DPFPImageQualityException e) {
                        System.out.printf("Failed to enroll the finger.\n");
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                }
            }
        }

        /**
         * Acquires fingerprint from the sensor and matches it with the registration templates
         * stored in the database.
         *
         * @param activeReader fingerprint reader to use
         */
        private void verify(String activeReader) {
            System.out.printf("Performing fingerprint verification...\n");
            String username = ShowDialog("Enter the name of the person to be verified: ");

            UserDatabase.User user = userDatabase.getUser(username);
            if (user == null) {
                System.out.printf("\"%s\" was not found in the database.\n", username);
            } else {
                if (user.isEmpty()) {
                    System.out.printf("No fingers for \"%s\" have been enrolled.\n", username);
                } else {
                    try {
                        DPFPSample sample = getSample(activeReader, "Scan your finger\n");
                        if (sample == null)
                            throw new Exception();

                        DPFPFeatureExtraction featureExtractor = DPFPGlobal.getFeatureExtractionFactory().createFeatureExtraction();
                        DPFPFeatureSet featureSet = featureExtractor.createFeatureSet(sample, DPFPDataPurpose.DATA_PURPOSE_VERIFICATION);

                        DPFPVerification matcher = DPFPGlobal.getVerificationFactory().createVerification();
                        matcher.setFARRequested(DPFPVerification.MEDIUM_SECURITY_FAR);
                        
                        for (DPFPFingerIndex finger : DPFPFingerIndex.values()) {
                            DPFPTemplate template = user.getTemplate(finger);
                            if (template != null) {
                                DPFPVerificationResult result = matcher.verify(featureSet, template);
                                if (result.isVerified()) {
                                    System.out.printf("Matching finger: %s, FAR achieved: %g.\n",
                                    		fingerName(finger), (double)result.getFalseAcceptRate()/DPFPVerification.PROBABILITY_ONE);
                                    return;
                                }
                            }
                        }
                    } catch (Exception e) {
                        System.out.printf("Failed to perform verification.");
                    }
                    System.out.printf("No matching fingers for \"%s\" were found.\n", username);
                }
            }
        }

        /**
         * Prints information on all available readers
         */
        private void listReaders() {
            DPFPReadersCollection readers = DPFPGlobal.getReadersFactory().getReaders();
            if (readers == null || readers.size() == 0) {
                System.out.printf("There are no readers available.\n");
                return;
            }

            System.out.printf("Available readers:\n");
            for (DPFPReaderDescription readerDescription : readers)
                System.out.println(readerDescription.getSerialNumber());
        }

        /**
         * selectReader() stores chosen reader in *pActiveReader
         * @param activeReader currently selected reader
         * @return reader selected
         * @throws IndexOutOfBoundsException if no reader available
         */
        String selectReader(String activeReader) throws IndexOutOfBoundsException {
            DPFPReadersCollection readers = DPFPGlobal.getReadersFactory().getReaders();
            if (readers == null || readers.size() == 0)
                throw new IndexOutOfBoundsException("There are no readers available");

            Vector<MenuItem> menu = new Vector<MenuItem>();
            for (DPFPReaderDescription readerDescription : readers)
                menu.add(new MenuItem(readerDescription.getSerialNumber(), menu.size()));
            menu.add(new MenuItem("Any available readers", menu.size()));

            int res = MenuShow(menu, MENU_WITH_BACK);
            if (res == backItem.getValue()) {
                return activeReader;
            } else if (res == readers.size()) {
                return null;
            } else {
                return readers.get(res).getSerialNumber();
            }
        }

        /**
         * Acquires a fingerprint sample from the specified fingerprint reader
         *
         * @param activeReader Fingerprint reader to use for acquisition
         * @return sample acquired
         * @throws InterruptedException if thread is interrupted
         */
        private DPFPSample getSample(String activeReader, String prompt)
        	throws InterruptedException
        {
            final LinkedBlockingQueue<DPFPSample> samples = new LinkedBlockingQueue<DPFPSample>();
            DPFPCapture capture = DPFPGlobal.getCaptureFactory().createCapture();
            capture.setReaderSerialNumber(activeReader);
            capture.setPriority(DPFPCapturePriority.CAPTURE_PRIORITY_LOW);
            capture.addDataListener(new DPFPDataListener()
            {
                public void dataAcquired(DPFPDataEvent e) {
                    if (e != null && e.getSample() != null) {
                        try {
                            samples.put(e.getSample());
                        } catch (InterruptedException e1) {
                            e1.printStackTrace();
                        }
                    }
                }
            });
            capture.addReaderStatusListener(new DPFPReaderStatusAdapter()
            {
            	int lastStatus = DPFPReaderStatusEvent.READER_CONNECTED;
				public void readerConnected(DPFPReaderStatusEvent e) {
					if (lastStatus != e.getReaderStatus())
						System.out.println("Reader is connected");
					lastStatus = e.getReaderStatus();
				}
				public void readerDisconnected(DPFPReaderStatusEvent e) {
					if (lastStatus != e.getReaderStatus())
						System.out.println("Reader is disconnected");
					lastStatus = e.getReaderStatus();
				}
            	
            });
            try {
                capture.startCapture();
                System.out.print(prompt);
                return samples.take();
            } catch (RuntimeException e) {
                System.out.printf("Failed to start capture. Check that reader is not used by another application.\n");
                throw e;
            } finally {
                capture.stopCapture();
            }
        }

        private String ShowDialog(String prompt) {
            System.out.printf(prompt);
            try {
                BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
                return stdin.readLine();
            } catch (IOException e) {
                return "";
            }
        }
        
        private String fingerName(DPFPFingerIndex finger) {
        	return fingerNames.get(finger); 
        }
        private String fingerprintName(DPFPFingerIndex finger) {
        	return fingerNames.get(finger) + " fingerprint"; 
        }
    }
}
