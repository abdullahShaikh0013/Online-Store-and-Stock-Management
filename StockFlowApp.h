#pragma once
#include "imgui.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

#include "Product.h"
#include "CartSystem.h"
#include "PaymentStrategies.h"
#include "User.h"

using namespace std;


struct Notification {
    string message;
    string timestamp;
    int    type;
};


struct CartEntry {
    int    productId;
    string productName;
    double price;
    int    qty;
    double total() const { return price * qty; }
};


struct OrderRecord {
    int    orderId = 0;
    string items = "";
    double total = 0.0;
    string payment = "";
    string timestamp = "";
};

static const ImVec4 COL_ACCENT{ 0.58f, 0.48f, 0.96f, 1.00f };
static const ImVec4 COL_GOLD{ 0.96f, 0.78f, 0.32f, 1.00f };
static const ImVec4 COL_GREEN{ 0.32f, 0.90f, 0.54f, 1.00f };
static const ImVec4 COL_RED{ 0.96f, 0.36f, 0.36f, 1.00f };
static const ImVec4 COL_MUTED{ 0.50f, 0.50f, 0.65f, 1.00f };
static const ImVec4 COL_WHITE{ 1.00f, 1.00f, 1.00f, 1.00f };
static const ImVec4 COL_BG_CARD{ 0.12f, 0.12f, 0.17f, 1.00f };
static const ImVec4 COL_BG_DARK{ 0.08f, 0.08f, 0.10f, 1.00f };


class StockFlowApp
{
public:
    StockFlowApp();
    ~StockFlowApp();
    void Render();

private:
    
    enum class Screen { Login, Admin, Customer };
    Screen currentScreen = Screen::Login;

  
    char   loginName[64] = "";
    int    loginId = 0;
    int    loginRole = 0; // 0=Admin 1=Customer
    string errorMsg;

    string loggedName;
    int    loggedId = 0;
    bool   isAdmin = false;

    Inventory* inventory = nullptr;
    Admin* adminUser = nullptr;
    Customer* custUser = nullptr;

    vector<Notification> notifications;
    bool showNotifPanel = false;
    int  unreadCount = 0;

    int  adminTab = 0;
    char apName[64] = "";
    int  apId = 0;
    float apPrice = 0.0f;
    int  apStock = 0;
    char apSearchFilter[64] = "";
    int  upId = 0;
    int  upQty = 0;
    int  custTab = 0;
    char catSearch[64] = "";
    vector<int>         cartQtyMap;
    vector<CartEntry>   cartEntries;
    int  payMethod = 0; // 0=Cash 1=Card
    vector<OrderRecord> orderHistory;
    bool checkoutSuccess = false;
    OrderRecord lastOrder;

    bool   showRemoveConfirm = false;
    int    removeTargetId = -1;
    string removeTargetName;
    void RenderLoginScreen();
    void RenderAdminScreen();
    void RenderCustomerScreen();
    void RenderTopBar();
    void RenderNotifPanel();
    void RenderStatCard(const char* label, const char* value,
        ImVec4 color, float width);

    void RenderAdminInventoryTab();
    void RenderAdminAddProductTab();
    void RenderAdminUpdateStockTab();

    // Customer tabs
    void RenderCatalogTab();
    void RenderCartTab();
    void RenderOrderHistoryTab();

    // Helpers
    void   DoLogin();
    void   DoLogout();
    void   AddNotification(const string& msg, int type = 0);
    string NowTime();
    void   SyncCartFromBackend();
    double CartTotal();
    int    CartItemCount();
};
