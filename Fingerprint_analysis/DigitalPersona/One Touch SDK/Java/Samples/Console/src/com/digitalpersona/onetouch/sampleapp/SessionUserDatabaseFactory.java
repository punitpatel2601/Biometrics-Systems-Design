package com.digitalpersona.onetouch.sampleapp;

import com.digitalpersona.onetouch.DPFPFingerIndex;
import com.digitalpersona.onetouch.DPFPTemplate;

import java.util.EnumMap;
import java.util.HashMap;
import java.util.Map;

/**
 * Implementation UserDatabase.Factory interface that produce instances of in-memory user database
 * without serialization.
 */
public class SessionUserDatabaseFactory implements UserDatabase.Factory {

    public UserDatabase createDB() {
        return new SessionUserDatabase();
    }

    /**
     * In-memory user database without serialization
     */
    private static class SessionUserDatabase implements UserDatabase {
        private Map<String, User> db = new HashMap<String, User>();

        /**
         * Adds a user to the database
         *
         * @param username user to be added
         * @return user added
         */
        public User addUser(String username) {
            User ret = new UserImpl(username);
            db.put(username, ret);
            return ret;
        }

        /**
         * Finds a user by name
         *
         * @param username user to find
         * @return user found, or <code>null</code> if not found)
         */
        public User getUser(String username) {
            return db.get(username);
        }

        /**
         * In-memory implementation of UserImpl interface
         */
        private static class UserImpl implements User {
            private String username;
            private EnumMap<DPFPFingerIndex, DPFPTemplate> templates;

            public UserImpl(String username) {
                this.username = username;
                templates = new EnumMap<DPFPFingerIndex, DPFPTemplate>(DPFPFingerIndex.class);
            }

            public String getUsername() {
                return username;
            }

            public DPFPTemplate getTemplate(DPFPFingerIndex finger) {
                return templates.get(finger);
            }

            public void setTemplate(DPFPFingerIndex finger, DPFPTemplate template) {
                templates.put(finger, template);
            }

            public boolean isEmpty() {
                return templates.isEmpty();
            }

        }
    }
}
