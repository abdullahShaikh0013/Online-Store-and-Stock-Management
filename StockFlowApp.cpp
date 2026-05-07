#include "StockFlowApp.h"
#include <cstring>
#include <cstdio>

StockFlowApp::StockFlowApp()
{
    srand((unsigned int)time(0));
    inventory = new Inventory();

    inventory->addProduct(new Product(101, "Laptop_Pro", 1299.99, 15));
    inventory->addProduct(new Product(102, "Wireless_Mouse", 29.99, 3));
    inventory->addProduct(new Product(103, "Mechanical_Keyboard", 149.99, 0));
    inventory->addProduct(new Product(104, "USB_C_Hub", 49.99, 22));
    inventory->addProduct(new Product(105, "Monitor_27inch", 399.99, 8));
    inventory->addProduct(new Product(106, "Headphones_Pro", 199.99, 5));
    inventory->addProduct(new Product(107, "Webcam_4K", 89.99, 12));
}

StockFlowApp::~StockFlowApp()
{
    delete adminUser;
    delete custUser;
    delete inventory;
}

string StockFlowApp::NowTime()
{
    time_t t = time(0);
    char buf[16];

    tm tm_info;                   
    localtime_s(&tm_info, &t);     

    strftime(buf, sizeof(buf), "%H:%M:%S", &tm_info);
    return string(buf);
}

void StockFlowApp::AddNotification(const string& msg, int type)
{
    notifications.push_back({ msg, NowTime(), type });
    unreadCount++;
}

double StockFlowApp::CartTotal()
{
    double t = 0;
    for (auto& e : cartEntries) t += e.total();
    return t;
}

int StockFlowApp::CartItemCount()
{
    int c = 0;
    for (auto& e : cartEntries) c += e.qty;
    return c;
}

void StockFlowApp::SyncCartFromBackend()
{
    int n = inventory->getCount();
    if ((int)cartQtyMap.size() != n)
        cartQtyMap.assign(n, 1);
}

void StockFlowApp::DoLogin()
{
    errorMsg.clear();
    if (loginId <= 0)
    {
        errorMsg = "Please enter a valid numeric ID."; return;
    }
    if (strlen(loginName) == 0)
    {
        errorMsg = "Please enter your name."; return;
    }

    loggedId = loginId;
    loggedName = string(loginName);
    isAdmin = (loginRole == 0);

    delete adminUser; adminUser = nullptr;
    delete custUser;  custUser = nullptr;

    if (isAdmin) {
        adminUser = new Admin(loggedId, loggedName);
        inventory->attach(adminUser);
        currentScreen = Screen::Admin;
    }
    else {
        custUser = new Customer(loggedId, loggedName);
        inventory->attach(custUser);
        cartEntries.clear();
        cartQtyMap.clear();
        SyncCartFromBackend();
        currentScreen = Screen::Customer;
    }

    notifications.clear();
    unreadCount = 0;
    AddNotification("Welcome, " + loggedName + "!", 1);
}

void StockFlowApp::DoLogout()
{
    delete adminUser; adminUser = nullptr;
    delete custUser;  custUser = nullptr;
    cartEntries.clear();
    cartQtyMap.clear();
    notifications.clear();
    unreadCount = 0;
    currentScreen = Screen::Login;
    memset(loginName, 0, sizeof(loginName));
    loginId = 0;
}

void StockFlowApp::Render()
{
    switch (currentScreen) {
    case Screen::Login:    RenderLoginScreen();    break;
    case Screen::Admin:    RenderAdminScreen();    break;
    case Screen::Customer: RenderCustomerScreen(); break;
    }
}

void StockFlowApp::RenderLoginScreen()
{
    ImGuiIO& io = ImGui::GetIO();
    float W = io.DisplaySize.x, H = io.DisplaySize.y;

    float cardW = 460, cardH = 420;
    ImGui::SetNextWindowPos(ImVec2((W - cardW) * 0.5f, (H - cardH) * 0.5f));
    ImGui::SetNextWindowSize(ImVec2(cardW, cardH));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_CARD);
    ImGui::Begin("##login", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    // Logo
    ImGui::Spacing();
    ImGui::SetCursorPosX(160);
    ImGui::SetWindowFontScale(1.8f);
    ImGui::TextColored(COL_ACCENT, "Stock");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(COL_WHITE, "Flow");
    ImGui::SetWindowFontScale(1.0f);

    ImGui::SetCursorPosX(100);
    ImGui::TextColored(COL_MUTED, "Online Stock Management System");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Role buttons
    ImGui::Text("Select Role:");
    ImGui::Spacing();
    float btnW = (cardW - 40) * 0.5f;

    if (loginRole == 0)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.24f, 0.55f, 1.f));
    else
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.13f, 0.13f, 0.18f, 1.f));
    if (ImGui::Button("  Admin  ", ImVec2(btnW, 36))) loginRole = 0;
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 8);

    if (loginRole == 1)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.24f, 0.55f, 1.f));
    else
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.13f, 0.13f, 0.18f, 1.f));
    if (ImGui::Button("  Customer  ", ImVec2(btnW, 36))) loginRole = 1;
    ImGui::PopStyleColor();

    ImGui::Spacing(); ImGui::Spacing();

    ImGui::Text("User ID");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputInt("##uid", &loginId);

    ImGui::Spacing();
    ImGui::Text("Name");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##uname", loginName, sizeof(loginName));

    ImGui::Spacing();
    if (!errorMsg.empty())
        ImGui::TextColored(COL_RED, "  %s", errorMsg.c_str());

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button,
        ImVec4(0.38f, 0.30f, 0.72f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
        ImVec4(0.48f, 0.40f, 0.88f, 1.f));
    if (ImGui::Button("  Enter System ->  ", ImVec2(-1, 42)))
        DoLogin();
    ImGui::PopStyleColor(2);

    ImGui::PopStyleColor();
    ImGui::End();
}


void StockFlowApp::RenderTopBar()
{
    ImGuiIO& io = ImGui::GetIO();
    float W = io.DisplaySize.x;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(W, 52));
    ImGui::PushStyleColor(ImGuiCol_WindowBg,
        ImVec4(0.09f, 0.09f, 0.12f, 0.97f));
    ImGui::Begin("##topbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::SetCursorPosY(12);
    ImGui::SetWindowFontScale(1.25f);
    ImGui::TextColored(COL_ACCENT, "Stock");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(COL_WHITE, "Flow");
    ImGui::SetWindowFontScale(1.0f);

    ImGui::SameLine(W - 340);
    ImGui::SetCursorPosY(13);

    // Bell button
    char bellLabel[32];
    if (unreadCount > 0) {
        snprintf(bellLabel, sizeof(bellLabel),
            " [!%d] ##bell", unreadCount);
        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.40f, 0.30f, 0.15f, 1.f));
    }
    else {
        snprintf(bellLabel, sizeof(bellLabel), " Bell ##bell");
        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.15f, 0.15f, 0.20f, 1.f));
    }
    if (ImGui::Button(bellLabel, ImVec2(72, 26))) {
        showNotifPanel = !showNotifPanel;
        unreadCount = 0;
    }
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 10);

    // User badge
    ImGui::TextColored(
        isAdmin ? COL_ACCENT : COL_GOLD,
        "[%s] %s",
        isAdmin ? "ADMIN" : "CUST",
        loggedName.c_str());

    ImGui::SameLine(0, 16);

    ImGui::PushStyleColor(ImGuiCol_Button,
        ImVec4(0.35f, 0.12f, 0.12f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
        ImVec4(0.55f, 0.18f, 0.18f, 1.f));
    if (ImGui::Button("Logout", ImVec2(65, 26))) DoLogout();
    ImGui::PopStyleColor(2);

    ImGui::End();
    ImGui::PopStyleColor();
}


void StockFlowApp::RenderNotifPanel()
{
    if (!showNotifPanel) return;
    ImGuiIO& io = ImGui::GetIO();
    float W = io.DisplaySize.x;

    ImGui::SetNextWindowPos(ImVec2(W - 370, 54));
    ImGui::SetNextWindowSize(ImVec2(360, 300));
    ImGui::PushStyleColor(ImGuiCol_WindowBg,
        ImVec4(0.11f, 0.11f, 0.16f, 0.98f));
    ImGui::Begin("##notif", &showNotifPanel,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(COL_ACCENT, "  Notifications");
    ImGui::Separator();
    ImGui::Spacing();

    if (notifications.empty()) {
        ImGui::TextColored(COL_MUTED, "  No notifications yet.");
    }
    else {
        ImGui::BeginChild("##nlist", ImVec2(-1, -1), false);
        for (int i = (int)notifications.size() - 1; i >= 0; i--) {
            auto& n = notifications[i];
            ImVec4 col = (n.type == 1) ? COL_GREEN :
                (n.type == 2) ? COL_GOLD :
                (n.type == 3) ? COL_RED : COL_ACCENT;
            ImGui::TextColored(col, "* ");
            ImGui::SameLine();
            ImGui::TextWrapped("%s", n.message.c_str());
            ImGui::TextColored(COL_MUTED, "  %s",
                n.timestamp.c_str());
            ImGui::Separator();
        }
        ImGui::EndChild();
    }
    ImGui::End();
    ImGui::PopStyleColor();
}


void StockFlowApp::RenderStatCard(
    const char* label, const char* value,
    ImVec4 color, float width)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
    ImGui::BeginChild(label, ImVec2(width, 72), true);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
    ImGui::TextColored(COL_MUTED, "%s", label);
    ImGui::SetWindowFontScale(1.4f);
    ImGui::TextColored(color, "%s", value);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor();
}
