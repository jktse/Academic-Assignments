package com.cmpt276.project.walkinggroupapp.model;

import android.content.Context;
import android.util.Log;

import com.cmpt276.project.walkinggroupapp.appactivities.ViewPermissionActivity;
import com.cmpt276.project.walkinggroupapp.proxy.ProxyBuilder;
import com.cmpt276.project.walkinggroupapp.proxy.WGServerProxy;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;

import java.io.IOException;
import java.security.Policy;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import retrofit2.Call;

/**
 * Created by zherenx on 2018-03-03.
 */

public class ModelManager {

    private static ModelManager instance = null;

    private String apiKey = null;

    private WGServerProxy proxy = null;

    private String token = null;

    private User user = null;











    public static ModelManager getInstance() {
        if (instance == null) {
            instance = new ModelManager();
        }
        return instance;
    }


    public void setApiKey(String apiKey) {
        this.apiKey = apiKey;
        proxy = ProxyBuilder.getProxy(apiKey, token);
    }



    public void register(Context context,
                         ProxyBuilder.SimpleCallback<Void> callback,
                         String name, String emailAddress, String password,
                         Integer birthYear, Integer birthMonth,
                         String address, String cellPhone,
                         String homePhone, String grade,
                         String teacherName, String emergencyContactInfo ) {


        User newUser = new User();
        newUser.setName(name);
        newUser.setEmail(emailAddress);
        newUser.setPassword(password);
        //Testing
        newUser.setBirthMonth(birthMonth);
        newUser.setBirthYear(birthYear);
        newUser.setAddress(address);
        newUser.setCellPhone(cellPhone);
        newUser.setHomePhone(homePhone);
        newUser.setGrade(grade);
        newUser.setTeacherName(teacherName);
        newUser.setEmergencyContactInfo(emergencyContactInfo);


        // for gamification
        newUser.setCurrentPoints(0);
        newUser.setTotalPointsEarned(0);

        Gamification custom = new Gamification();
        newUser.setGamification(custom);
        try {
            // Convert custom object to a JSON string:
            String customAsJson = new ObjectMapper().writeValueAsString(custom);
            // Store JSON string into the user object, which will be sent to server.
            newUser.setCustomJson(customAsJson);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }


        Call<User> caller = proxy.createNewUser(newUser);
        ProxyBuilder.callProxy(context, caller, returnedUser -> {
            user = returnedUser;

            // this if condition is just a guard, it shouldn't get executed unless something else went wrong.
            if (user.getCustomJson() == null) {
                user.setCustomJson("{\"currentAvatar\":0,\"ownedAvatars\":[0]}");
            }

            // Deserialize the custom object from the user:
            try {
                Gamification customObjectFromServer = new ObjectMapper().readValue(user.getCustomJson(), Gamification.class);
                user.setGamification(customObjectFromServer);
            } catch (IOException e) {
                e.printStackTrace();
            }

            callback.callback(null);
        });

    }


    public void login(Context context,
                      ProxyBuilder.SimpleCallback<Void> onResponseCallback,
                      ProxyBuilder.SimpleCallback<String> onFailureCallback,
                      String emailAddress, String password) {
        user = new User();
        user.setEmail(emailAddress);
        user.setPassword(password);
        ProxyBuilder.setOnTokenReceiveCallback(token -> onReceiveToken(token));
        Call<Void> caller = proxy.login(user);
        ProxyBuilder.callProxy(
                context,
                caller,
                returnNothing -> {
                    Call<User> getUserCaller = proxy.getUserByEmail(user.getEmail());
                    ProxyBuilder.callProxy(context, getUserCaller, returnedUser -> {
                        user = returnedUser;

//                        // this if condition is just a guard, it shouldn't get executed unless something else went wrong.
//                        if (user.getCustomJson() == null) {
//                            user.setCustomJson("{\"currentAvatar\":0,\"ownedAvatars\":[0]}");
//                        }
//
//                        // Deserialize the custom object from the user:
//                        try {
//                            Gamification customObjectFromServer = new ObjectMapper().readValue(user.getCustomJson(), Gamification.class);
//                            user.setGamification(customObjectFromServer);
//                        } catch (IOException e) {
//                            e.printStackTrace();
//                        }

                        onResponseCallback.callback(null);
                    });
                },
                onFailureCallback
        );
    }

    private void onReceiveToken(String token) {
        proxy = ProxyBuilder.getProxy(apiKey, token);
    }



    public long getLocalUserId() {
        return user.getId();
    }


    public void getMonitorsUsers(Context context, ProxyBuilder.SimpleCallback<List<User>> callback) {
        Call<List<User>> caller = proxy.getMonitorsUsersById(user.getId());
        ProxyBuilder.callProxy(context, caller, returnedUsersList -> {
            for (User returnedUser: returnedUsersList) {

                // this if condition is just a guard, it shouldn't get executed unless something else went wrong.
                if (returnedUser.getCustomJson() == null) {
                    returnedUser.setCustomJson("{\"currentAvatar\":0,\"ownedAvatars\":[0]}");
                }

                // Deserialize the custom object from the user:
                try {
                    Gamification customObjectFromServer = new ObjectMapper().readValue(returnedUser.getCustomJson(), Gamification.class);
                    returnedUser.setGamification(customObjectFromServer);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            callback.callback(returnedUsersList);
        });
    }

    public void getMonitoredByUsers(Context context, ProxyBuilder.SimpleCallback<List<User>> callback) {
        Call<List<User>> caller = proxy.getMonitoredByUsersById(user.getId());
        ProxyBuilder.callProxy(context, caller, returnedUsersList -> {
            for (User returnedUser: returnedUsersList) {

                // this if condition is just a guard, it shouldn't get executed unless something else went wrong.
                if (returnedUser.getCustomJson() == null) {
                    returnedUser.setCustomJson("{\"currentAvatar\":0,\"ownedAvatars\":[0]}");
                }

                // Deserialize the custom object from the user:
                try {
                    Gamification customObjectFromServer = new ObjectMapper().readValue(returnedUser.getCustomJson(), Gamification.class);
                    returnedUser.setGamification(customObjectFromServer);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            callback.callback(returnedUsersList);
        });
    }



    public void getUserById(Context context,
                            ProxyBuilder.SimpleCallback<User> callback,
                            long userId) {
        Call<User> caller = proxy.getUserById(userId);
        ProxyBuilder.callProxy(context, caller, returnedUser -> {

            // this if condition is just a guard, it shouldn't get executed unless something else went wrong.
            if (returnedUser.getCustomJson() == null) {
                returnedUser.setCustomJson("{\"currentAvatar\":0,\"ownedAvatars\":[0]}");
            }

            // Deserialize the custom object from the user:
            try {
                Gamification customObjectFromServer = new ObjectMapper().readValue(returnedUser.getCustomJson(), Gamification.class);
                returnedUser.setGamification(customObjectFromServer);
            } catch (IOException e) {
                e.printStackTrace();
            }
            callback.callback(returnedUser);
        });
    }

    public void getUserById(Context context,
                            ProxyBuilder.SimpleCallback<User> callback,
                            ProxyBuilder.SimpleCallback<String> onFailureCallback,
                            long userId) {
        Call<User> caller = proxy.getUserById(userId);
        ProxyBuilder.callProxy(context, caller, returnedUser -> {

            // this if condition is just a guard, it shouldn't get executed unless something else went wrong.
            if (returnedUser.getCustomJson() == null) {
                returnedUser.setCustomJson("{\"currentAvatar\":0,\"ownedAvatars\":[0]}");
            }

            // Deserialize the custom object from the user:
            try {
                Gamification customObjectFromServer = new ObjectMapper().readValue(returnedUser.getCustomJson(), Gamification.class);
                returnedUser.setGamification(customObjectFromServer);
            } catch (IOException e) {
                e.printStackTrace();
            }
            callback.callback(returnedUser);
        }, onFailureCallback);
    }

    // this method is used to update points.
    public void addPoints(Context context, ProxyBuilder.SimpleCallback<User> callback, int pointsEarned) {
        Call<User> getUserCaller = proxy.getUserById(user.getId());
        ProxyBuilder.callProxy(context, getUserCaller, targetUser -> {
            int totalPoints = targetUser.getTotalPointsEarned() + pointsEarned;
            int currentPoints = targetUser.getCurrentPoints() + pointsEarned;
            targetUser.setTotalPointsEarned(totalPoints);
            targetUser.setCurrentPoints(currentPoints);
            Call<User> editUserCaller = proxy.editUserWithId(targetUser.getId(), targetUser);
            ProxyBuilder.callProxy(context, editUserCaller, returnedUser -> {
                // Deserialize the custom object from the user:
                try {
                    Gamification customObjectFromServer = new ObjectMapper().readValue(returnedUser.getCustomJson(), Gamification.class);
                    returnedUser.setGamification(customObjectFromServer);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                callback.callback(returnedUser);
            });
        });
    }

}
