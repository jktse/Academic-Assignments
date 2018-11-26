package com.cmpt276.project.walkinggroupapp.appactivities;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.PopupMenu;
import android.widget.TextView;

import com.cmpt276.project.walkinggroupapp.R;
import com.cmpt276.project.walkinggroupapp.model.MapState;
import com.cmpt276.project.walkinggroupapp.model.Message;
import com.cmpt276.project.walkinggroupapp.model.ModelManager;
import com.cmpt276.project.walkinggroupapp.model.User;
import com.cmpt276.project.walkinggroupapp.proxy.ProxyBuilder;

import java.util.List;

/******
 *
 * Main Menu Activity
 *  Notice the complex List Adapter is based off from https://www.youtube.com/watch?v=WRANgDgM2Zg
 *  a video provided by the prof for assignment 2
 *  Code is adapted from Jacky.T  Assignment 2
 */


public class MainMenuActivity extends AppCompatActivity {

    public static final String PREFERENCE_EMAIL = "saved.email.key";
    public static final String PREFERENCE_PASSWORD = "saved.password.key";
    public static final String PREFERENCE_IS_LOGOUT = "saved.logout.key";

    private Button btnAddNewMonitorsUser;
    private Button btnAddNewMonitoredByUser;
    private Button btnLogout;
    private Button btnViewGroup;
    private Button btnMessage;
    private Button btnParentDashboard;
    private Button btnShop;
    private Button btnPermission;

    private Button btnViewProfileLocal;

    private TextView unreadMessageTextView;

    private ListView monitorsUsersListView;
    private ListView monitoredByUsersListView;

    private List<User> monitorsUsers;
    private List<User> monitoredByUsers;

    private ImageButton leaderBoardButton;



    private ModelManager modelManager;

    private MapState mapState;




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_menu);

        modelManager = ModelManager.getInstance();
        mapState = MapState.getInstance();


        ProxyBuilder.SimpleCallback<List<User>> getMonitorsUsersCallback = monitorsUsers -> getMonitorsUsersResponse(monitorsUsers);
        modelManager.getMonitorsUsers(MainMenuActivity.this, getMonitorsUsersCallback);

        ProxyBuilder.SimpleCallback<List<User>> getMonitoredByUsersCallback = monitoredByUsers -> getMonitoredByUsersResponse(monitoredByUsers);
        modelManager.getMonitoredByUsers(MainMenuActivity.this, getMonitoredByUsersCallback);

        setupViewProfileLocalButton();

            setupAddNewMonitorsUserButton();
            setupAddNewMonitoredByUserButton();
            setupViewGroupButton();
            setupLogoutButton();
            setupMessageButton();
            setupParentDashboardButton();
            setupNumberOfUnreadMessages();
            setupShopButton();
            setupLeaderBoard();
            setupPermissionButton();

    }

    private void setupLeaderBoard() {
        leaderBoardButton = findViewById(R.id.anmol_leaderBoardButton);
        leaderBoardButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = LeaderBoardActivity.makeIntent(MainMenuActivity.this);
                startActivity(intent);
            }
        });
    }

    private void setupViewProfileLocalButton() {
        btnViewProfileLocal = findViewById(R.id.justin_btnViewProfileLocal);
        btnViewProfileLocal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = ViewLocalUserProfileActivity.makeIntent(getApplicationContext());
                startActivity(intent);
            }
        });
    }

    @Override
    public void onResume()
    {
        super.onResume();
        // TODO
        setupNumberOfUnreadMessages();
        ProxyBuilder.SimpleCallback<List<User>> getMonitorsUsersCallback = monitorsUsers -> getMonitorsUsersResponse(monitorsUsers);
        modelManager.getMonitorsUsers(MainMenuActivity.this, getMonitorsUsersCallback);

        ProxyBuilder.SimpleCallback<List<User>> getMonitoredByUsersCallback = monitoredByUsers -> getMonitoredByUsersResponse(monitoredByUsers);
        modelManager.getMonitoredByUsers(MainMenuActivity.this, getMonitoredByUsersCallback);
    }

    private void setupMessageButton(){
        btnMessage = findViewById(R.id.jacky_message_button);
        btnMessage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = MessageActivity.makeIntent(MainMenuActivity.this);
                startActivity(intent);
            }
        });
    }

    private  void setupShopButton() {
        btnShop = findViewById(R.id.gerry_Shop_Button_main);
        btnShop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //go to shop activity
                Intent intent = new Intent(MainMenuActivity.this, ShopActivity.class);
                startActivity(intent);
            }
        });
    }

    private void setupLogoutButton() {
        btnLogout = findViewById(R.id.gerry_Logout_Button_main);
        btnLogout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SharedPreferences sharedPreferences = getSharedPreferences("MyData", MODE_PRIVATE);
                SharedPreferences.Editor editor = sharedPreferences.edit();

                editor.clear();

                editor.putBoolean(PREFERENCE_IS_LOGOUT, true);

                editor.putString(PREFERENCE_EMAIL, null);
                editor.putString(PREFERENCE_PASSWORD, null);

                editor.commit();

                Intent intent = LoginActivity.makeIntent(getApplicationContext());
                startActivity(intent);

                finish();

            }
        });
    }

    private void setupParentDashboardButton () {
        btnParentDashboard = findViewById(R.id.gerry_Parent_Dashboard_Button_main);
        btnParentDashboard.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //go to map and show all children location
                //set properties to let map know what to do
                //set to let map know what to do
                MapState.CurrentStateEnum currentState = MapState.CurrentStateEnum.IsViewingAllChild;
                mapState.setCurrentStateEnum(currentState);

                //go to map activity
                Intent intent = new Intent(MainMenuActivity.this, MapActivity.class);
                startActivity(intent);
            }
        });
    }

    private void setupAddNewMonitorsUserButton() {
        //register button
        btnAddNewMonitorsUser = findViewById(R.id.jacky_add_monitoring_button);
        btnAddNewMonitorsUser.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = AddMonitorsUserActivity.makeIntent(getApplicationContext());
                startActivity(intent);
            }
        });
    }

    private void setupAddNewMonitoredByUserButton(){
        btnAddNewMonitoredByUser = findViewById(R.id.jacky_add_monitoring_by_button);
        btnAddNewMonitoredByUser.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = AddMonitoredByUserActivity.makeIntent(getApplicationContext());
                startActivity(intent);
            }
        });
    }

    private void setupViewGroupButton() {
        btnViewGroup = findViewById(R.id.jacky_view_groups);
        btnViewGroup.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = ViewGroupActivity.makeIntent(getApplicationContext());
                startActivity(intent);
            }
        });
    }

    private void setupNumberOfUnreadMessages(){
        unreadMessageTextView = findViewById(R.id.jacky_number_of_unread);
        ProxyBuilder.SimpleCallback<List<Message>> getUnreadMessagesCallback = unreadMessageList -> getUnreadMessagesResponse(unreadMessageList);
        modelManager.getUnreadMessagesForUser(MainMenuActivity.this, getUnreadMessagesCallback);
    }

    private void setupPermissionButton(){
        btnPermission = findViewById(R.id.jacky_permission_button);
        btnPermission.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = CheckPermissionActivity.makeIntent(getApplicationContext());
                startActivity(intent);
            }
        });
    }

    private void getUnreadMessagesResponse(List<Message> unreadMessageList){
        String numberOfUnreadMessagesString = "" + unreadMessageList.size();
        unreadMessageTextView.setText(numberOfUnreadMessagesString);
    }

    private void getMonitorsUsersResponse(List<User> monitorsUsers) {
        Log.i("MyApp","Inside update you");
        this.monitorsUsers = monitorsUsers;
        populateMonitorsUsersList();
        registerMonitorsUsersOnItemClick();
    }

    private void populateMonitorsUsersList() {
        ArrayAdapter<User> adapter = new monitorsUsersAdapter();
        //Configure ListView
        monitorsUsersListView = findViewById(R.id.jacky_monitoring_list);
        monitorsUsersListView.setAdapter(adapter);
        //Toast.makeText(getApplicationContext(), "Done Populating List", Toast.LENGTH_LONG).show();
    }

    private class monitorsUsersAdapter extends ArrayAdapter<User> {                                                 //Code for complexList based from Brian Frasers video
        public monitorsUsersAdapter() {
            super(MainMenuActivity.this, R.layout.list_layout, monitorsUsers);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            //Make sure We are given a view
            View itemView = convertView;
            if (itemView == null) {
                itemView = getLayoutInflater().inflate(R.layout.list_layout, parent, false);
            }
            //Find a user to add
            User currentUser = monitorsUsers.get(position);

            //Name:
            TextView makeName = itemView.findViewById(R.id.jacky_user_name_dynamic);
            makeName.setText(currentUser.getName());

            //Email
            TextView makeEmail = itemView.findViewById(R.id.jacky_user_email_dynamic);
            makeEmail.setText(currentUser.getEmail());

            //Icon
            ImageView setImage = itemView.findViewById(R.id.jacky_user_icon);
            if(position%5  == 0){
                setImage.setImageResource(R.drawable.dog_icon);
            }else if(position%5 == 1){
                setImage.setImageResource(R.drawable.dinosaur_icon);
            }else if(position%5 == 2){
                setImage.setImageResource(R.drawable.cat_icon);
            }else if(position%5 == 3){
                setImage.setImageResource(R.drawable.dolphin_icon);
            }else if(position%5 == 4){
                setImage.setImageResource(R.drawable.penguin_icon);
            } else{
                setImage.setImageResource(R.drawable.read_icon);
            }

            return itemView;
        }
    }

    private void getMonitoredByUsersResponse(List<User> monitoredByUsers) {
        Log.i("MyApp", "How many times CALLED???");
        this.monitoredByUsers = monitoredByUsers;
        populateMonitoredByUsersList();
        registerMonitoredByUsersOnItemClick();
    }

    private void populateMonitoredByUsersList() {
        ArrayAdapter<User> adapter = new monitoredByUsersAdapter();
        //Configure ListView
        monitoredByUsersListView = findViewById(R.id.jacky_monitoing_by_list);
        monitoredByUsersListView.setAdapter(adapter);
        //Toast.makeText(getApplicationContext(), "Done Populating List", Toast.LENGTH_LONG).show();
    }

    private class monitoredByUsersAdapter extends ArrayAdapter<User> {                                                 //Code for complexList based from Brian Frasers video
        public monitoredByUsersAdapter() {
            super(MainMenuActivity.this, R.layout.list_layout, monitoredByUsers);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            //Make sure We are given a view
            View itemView = convertView;
            if (itemView == null) {
                itemView = getLayoutInflater().inflate(R.layout.list_layout, parent, false);
            }
            //Find a user to add
            Log.i("MyApp", "Inside Monitoring By");
            User currentUser = monitoredByUsers.get(position);

            //Name:
            TextView makeName = itemView.findViewById(R.id.jacky_user_name_dynamic);
            makeName.setText(currentUser.getName());

            //Email
            TextView makeEmail = itemView.findViewById(R.id.jacky_user_email_dynamic);
            makeEmail.setText(currentUser.getEmail());

            //Icon
            ImageView setImage = itemView.findViewById(R.id.jacky_user_icon);
            if(position%5  == 0){
                setImage.setImageResource(R.drawable.dog_icon);
            }else if(position%5 == 1){
                setImage.setImageResource(R.drawable.dinosaur_icon);
            }else if(position%5 == 2){
                setImage.setImageResource(R.drawable.cat_icon);
            }else if(position%5 == 3){
                setImage.setImageResource(R.drawable.dolphin_icon);
            }else if(position%5 == 4){
                setImage.setImageResource(R.drawable.penguin_icon);
            } else{
                setImage.setImageResource(R.drawable.read_icon);
            }

            return itemView;
        }
    }

    public static Intent makeIntent(Context context){
        return new Intent(context, MainMenuActivity.class);
    }


    private void registerMonitorsUsersOnItemClick()                                                                                    //For clicking on list object
    {
        final ListView list = findViewById(R.id.jacky_monitoring_list);

        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View viewClicked, int position, long id) {
                long userId = monitorsUsers.get(position).getId();

                Intent intent = ViewChildProfileActivity.makeIntent(MainMenuActivity.this, userId);
                startActivity(intent);
            }
        });

        list.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {

            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View viewClicked, int position, long id)
            {
                //Toast.makeText(getApplicationContext(), "Pressed Long to edit" + position, Toast.LENGTH_SHORT).show();
                Log.i("MyApp", "Pressed Long" + position);
                    PopupMenu popupMenu = new PopupMenu(MainMenuActivity.this, viewClicked);
                popupMenu.getMenuInflater().inflate(R.menu.monitoring_user_popup, popupMenu.getMenu());

                popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {          //Code from https://www.youtube.com/watch?v=LXUDqGaToe0
                        @Override
                        public boolean onMenuItemClick(MenuItem menuItem) {

                            switch(menuItem.getItemId())
                            {
                                case R.id.delete:
                                    removeMonitorsUserByPosition(position);
                                    break;
                                case R.id.editGroup:
                                    editUserGroup(position);
                                    break;
                                case R.id.jacky_edit_user_information:
                                    editUserProfile(position);
                                    break;
                            }
                            return true;
                        }
                });

                popupMenu.show();
                return true;
            }
        });
    }

    private void registerMonitoredByUsersOnItemClick()                                                                                    //For clicking on list object
    {
        final ListView list = findViewById(R.id.jacky_monitoing_by_list);

        list.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {

            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View viewClicked, int position, long id)
            {
                //Toast.makeText(getApplicationContext(), "Pressed Long to edit" + position, Toast.LENGTH_SHORT).show();
                Log.i("MyApp", "Pressed Long" + position);
                PopupMenu popupMenu = new PopupMenu(MainMenuActivity.this, viewClicked);
                popupMenu.getMenuInflater().inflate(R.menu.popup_menu, popupMenu.getMenu());

                popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {          //Code from https://www.youtube.com/watch?v=LXUDqGaToe0
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {

                        switch(menuItem.getItemId())
                        {
                            case R.id.cancel:
                                break;
                            case R.id.delete:
                                removeMonitoredByUserByPosition(position);
                                break;
                        }
                        return true;
                    }

                });

                popupMenu.show();
                return true;
            }
        });
    }



    private void editUserGroup(int position) {
        long targetId = monitorsUsers.get(position).getId();
        Intent intent = EditMonitoringUserGroupActivity.makeIntent(getApplicationContext(), targetId);
        startActivity(intent);
    }

    private void editUserProfile(int position){
        long targetId = monitorsUsers.get(position).getId();
        Intent intent = EditProfileActivity.makeIntent(MainMenuActivity.this, targetId);
        startActivity(intent);
    }

    private void removeMonitorsUserByPosition(int position) {
        long targetId = monitorsUsers.get(position).getId();
        ProxyBuilder.SimpleCallback<Void> callback = returnNothing -> removeMonitorsUserResponse(returnNothing);
        modelManager.removeMonitorsUser(MainMenuActivity.this, callback, targetId);
    }

    private void removeMonitorsUserResponse(Void returnNothing) {
        ProxyBuilder.SimpleCallback<List<User>> getMonitorsUsersCallback = monitorsUsers -> getMonitorsUsersResponse(monitorsUsers);
        modelManager.getMonitorsUsers(MainMenuActivity.this, getMonitorsUsersCallback);
    }

    private void removeMonitoredByUserByPosition(int position) {
        long targetId = monitoredByUsers.get(position).getId();
        ProxyBuilder.SimpleCallback<Void> callback = returnNothing -> removeMonitoredByUserResponse(returnNothing);
        modelManager.removeMonitoredByUser(MainMenuActivity.this, callback, targetId);
    }

    private void removeMonitoredByUserResponse(Void returnNothing) {
        ProxyBuilder.SimpleCallback<List<User>> getMonitoredByUsersCallback = monitoredByUsers -> getMonitoredByUsersResponse(monitoredByUsers);
        modelManager.getMonitoredByUsers(MainMenuActivity.this, getMonitoredByUsersCallback);
    }


    @Override
    public void onBackPressed() {
        //super.onBackPressed();
        this.moveTaskToBack(true);
    }
}
