package com.digitalpersona.onetouch.sampleapp;

import com.digitalpersona.onetouch.DPFPFingerIndex;
import com.digitalpersona.onetouch.DPFPTemplate;

/**
 * User database
 */
public interface UserDatabase {

    /**
     * User database factory
     */
    public static interface Factory {
        UserDatabase createDB();
    }

    /**
     * Fingerprint database user
     */
    public static interface User {

        /**
         * Gets the user name
         *
         * @return user name
         */
        public String getUsername();

        /**
         * Returns a template associated with a specific finger
         *
         * @param finger finger index
         * @return fingerprint template
         */
        public DPFPTemplate getTemplate(DPFPFingerIndex finger);

        /**
         * Adds or replaces a fingerprint template associated with fingerprint
         *
         * @param finger   finger index
         * @param template fingerprint template
         */
        public void setTemplate(DPFPFingerIndex finger, DPFPTemplate template);

        /**
         * Checks if user has any fingerprint set
         *
         * @return <code>true</code> if no fingerprint templates stored
         */
        public boolean isEmpty();
    }

    /**
     * Adds a user to the database
     *
     * @param username user name
     * @return user added
     */
    public User addUser(String username);

    /**
     * Finds a user by name
     *
     * @param username user to find
     * @return user found, or <code>null</code> if not found)
     */
    public User getUser(String username);

}
