package com.digitalpersona.onetouch.sampleapp;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


/**
 * Creates UserInterface and UserDatabase instances and runs UserInterface.
 */
public class SampleApp {
    private final static String DEFAULT_UI_FACTORY = "com.digitalpersona.onetouch.sampleapp.ConsoleUserInterfaceFactory";
    private final static String DEFAULT_DB_FACTORY = "com.digitalpersona.onetouch.sampleapp.SessionUserDatabaseFactory";

    public static void main (String[] args) {

        ///TODO: parse command line to allow user to specify other factories
        String uiFactoryName = DEFAULT_UI_FACTORY;
        String dbFactoryName = DEFAULT_DB_FACTORY;

        try {
            UserDatabase.Factory dbFactory = (UserDatabase.Factory)Class.forName(dbFactoryName).newInstance();
            UserDatabase userDatabase = dbFactory.createDB();

            UserInterface.Factory uiFactory = (UserInterface.Factory)Class.forName(uiFactoryName).newInstance();
            UserInterface userInterface = uiFactory.createUI(userDatabase);

            ExecutorService exec = Executors.newCachedThreadPool();
            exec.execute(userInterface);

            exec.shutdown();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
