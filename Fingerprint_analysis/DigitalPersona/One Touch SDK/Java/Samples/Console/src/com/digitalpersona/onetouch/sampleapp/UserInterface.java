package com.digitalpersona.onetouch.sampleapp;

/**
 * Abstract user interface
 */
public interface UserInterface extends Runnable {

    /**
     * User interface factory
     */
    public static interface Factory {
        /**
         * Creates an object implementing UserInterface interface
         * @param userDatabase user database to be used with the ui
         * @return created instance
         */
        UserInterface createUI(UserDatabase userDatabase);
    }
}
