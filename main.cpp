#include <iostream>
#include <ctime>

#include "Product.h"
#include "CartSystem.h"
#include "PaymentStrategies.h"
#include "User.h"

using namespace std;

int main() {
    srand((unsigned int)time(0));

    cout << "=========================================\n";
    cout << "   ONLINE CLOTHING & STOCK SYSTEM\n";
    cout << "=========================================\n\n";

 
    Inventory inventory;

    Admin admin(1, "Admin_Ali");
    Customer customer(101, "Customer_Sara");

    // Attach observers
    inventory.attach(&admin);
    inventory.attach(&customer);

    cout << "\n--- Adding Products (Observer Trigger) ---\n";

    inventory.addProduct(new Product(1, "Shirt", 25.5, 10));
    inventory.addProduct(new Product(2, "Jeans", 40.0, 8));
    inventory.addProduct(new Product(3, "Jacket", 80.0, 5));
    inventory.addProduct(new Product(4, "Shoes", 60.0, 6));

    cout << "\n--- Admin Updating Stock ---\n";

    inventory.updateStock(2, 5);    // Restock Jeans
    inventory.updateStock(3, -2);   // Reduce Jacket
    inventory.updateStock(3, -3);   // Should trigger LOW / CRITICAL

    
    cout << "\n--- Current Catalog ---\n";
    for (int i = 0; i < inventory.getCount(); i++) {
        Product* p = inventory.getProductAt(i);
        cout << "[" << p->getId() << "] "
            << p->getName() << " - $"
            << p->getPrice() << " ("
            << p->getStock() << " left)\n";
    }

    cout << "\n--- Customer Adding Items to Cart ---\n";

    Cart& cart = customer.getCart();

    Product* p1 = inventory.findProduct(1);
    Product* p2 = inventory.findProduct(2);

    if (p1 && p1->getStock() >= 2)
        cart.addItem(new CartItem(*p1, 2));

    if (p2 && p2->getStock() >= 3)
        cart.addItem(new CartItem(*p2, 3));

    
    cout << "\n--- Cart Details ---\n";

    for (int i = 0; i < cart.getItemsCount(); i++) {
        CartComponent* item = cart.getItemAt(i);
        cout << "Item " << i + 1 << " Price: $" << item->getPrice() << endl;
    }

    cout << "Total Cart Price: $" << cart.getPrice() << endl;

    cout << "\n--- Checkout ---\n";

    int itemCount = cart.getItemsCount();

    CartItem** extractedItems = new CartItem * [itemCount];
    for (int i = 0; i < itemCount; i++) {
        extractedItems[i] = (CartItem*)cart.getItemAt(i);
    }

    int orderID = rand() % 1000 + 1000;

    Order* order = new Order(orderID, extractedItems, itemCount, cart.getPrice());

    delete[] extractedItems;

  
    cout << "\n--- Payment ---\n";

    PaymentMethod* payment = new CashPayment();  // try CardPayment also
    order->processOrder(payment);

    cout << "\n--- Updating Stock After Purchase ---\n";

    for (int i = 0; i < itemCount; i++) {
        CartItem* item = (CartItem*)cart.getItemAt(i);
        inventory.updateStock(
            item->getProduct().getId(),
            -item->getQuantity()
        );
    }

    cart.clearCart();

    cout << "\n--- Admin Removing Product ---\n";
    inventory.removeItem(4); // Remove Shoes

    cout << "\n--- Edge Case Testing ---\n";

    inventory.updateStock(999, 5);   // Invalid product
    inventory.removeItem(999);       // Invalid removal

    cout << "\n--- Last Notifications ---\n";
    cout << "Admin: " << admin.getLastNotification() << endl;
    cout << "Customer: " << customer.getLastNotification() << endl;

    delete payment;
    delete order;

    cout << "\n=========================================\n";
    cout << "   SYSTEM EXECUTION COMPLETED\n";
    cout << "=========================================\n";

    return 0;
}

