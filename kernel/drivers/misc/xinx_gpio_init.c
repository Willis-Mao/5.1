#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of_gpio.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <linux/kernel.h>  
#include <linux/module.h>  
#include <linux/init.h>  
#include <linux/fs.h>  
#include <linux/string.h>  
#include <linux/mm.h>  
#include <linux/syscalls.h>  
#include <asm/unistd.h>  
#include <asm/uaccess.h>  

#define INVALID_GPIO 	-1

struct qy_config{
	int pin;
	int active_level;
};

struct qyboard_priv {
	struct qy_config buzzer_ctrl;
	struct qy_config user_led;
	struct qy_config amp_on;
	struct qy_config lte_rst;
	struct qy_config lte_flight;
	struct qy_config hub_rst;
	struct qy_config dc5v0_en;
	struct qy_config dc3v3_en;
	struct qy_config dc7v0_en;
	struct qy_config dvp_en;
	struct qy_config dc12v0_en;
	struct qy_config hardware_ver0;
	struct qy_config hardware_ver1;
	struct qy_config hardware_ver2;
};

struct user_gpio_config {
	struct gpio *gpios;
	int nr_gpios;
};

static int reserved_gpio_init(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct user_gpio_config config;
	enum of_gpio_flags flags;
	int ret = 0;
	int gpio, i;	
	
	/* Fetch GPIOs. */
	config.nr_gpios = of_gpio_named_count(np, "user-gpios");

	config.gpios = devm_kzalloc(&pdev->dev,
				sizeof(struct gpio) * config.nr_gpios,
				GFP_KERNEL);
	if (!config.gpios){
		ret = -ENOMEM;
		goto failed;
	}
	for (i = 0; i < config.nr_gpios; i++) {
		gpio = of_get_named_gpio_flags(np, "user-gpios", i, &flags);
		if (gpio < 0)
			break;
		config.gpios[i].gpio = gpio;	
		config.gpios[i].flags = flags & OF_GPIO_ACTIVE_LOW;
		
		printk("%s:gpio[%d] = %d, value = %lu\n",__func__, i, gpio, config.gpios[i].flags);
	}

	ret = gpio_request_array(config.gpios, config.nr_gpios);
	if (ret) {
		dev_err(&pdev->dev, "Could not obtain init GPIOs: %d\n", ret);
		goto failed;
	}

	for(i=0; i<config.nr_gpios; i++)
	{
		gpio_direction_output(config.gpios[i].gpio, config.gpios[i].flags);			
		gpio_export(config.gpios[i].gpio, 1);
	}

failed:
    devm_kfree(&pdev->dev, config.gpios);

	return ret;
}
static int gpio_init_probe(struct platform_device *pdev)
{
	struct qyboard_priv *qyboard = NULL;
	struct device_node *np = pdev->dev.of_node;
	enum of_gpio_flags flags;
	int ver[3];	
	int ret;
	int i;

	printk("************gpio_init_probe\n");
	if (!np) {
		dev_err(&pdev->dev, "Missing device tree node.\n");
		return -EINVAL;
	}

	qyboard = devm_kzalloc(&pdev->dev, sizeof(struct qyboard_priv), GFP_KERNEL);
	if (!qyboard) {
		dev_err(&pdev->dev, "kmalloc for qyboard fail!");
		return -ENOMEM;
	}
	platform_set_drvdata(pdev, qyboard);
	
	/* 3.3v for board enable */
	qyboard->dc3v3_en.pin = of_get_named_gpio_flags(np, "dc3v3-en", 0, &flags);
	if (qyboard->dc3v3_en.pin < 0) {
		printk("%s() Can not read property dc3v3_en.pin\n", __FUNCTION__);
		qyboard->dc3v3_en.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->dc3v3_en.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->dc3v3_en.pin, "dc3v3-en");
	    if (ret != 0) {
		    printk("%s request dc3v3_en error", __func__);
	    }
		gpio_direction_output(qyboard->dc3v3_en.pin, qyboard->dc3v3_en.active_level);
								gpio_export(qyboard->dc3v3_en.pin, 0);

		printk("*****dc3v3-en %d init ok, active_level = %d.\n",qyboard->dc3v3_en.pin, qyboard->dc3v3_en.active_level);
	}
	
	/* 5v for board enable */
	qyboard->dc5v0_en.pin = of_get_named_gpio_flags(np, "dc5v0-en", 0, &flags);
	if (qyboard->dc5v0_en.pin < 0) {
		printk("%s() Can not read property lte_rst.pin\n", __FUNCTION__);
		qyboard->dc5v0_en.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->dc5v0_en.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->dc5v0_en.pin, "dc5v0-en");
	    if (ret != 0) {
		    printk("%s request dc5v0_en error", __func__);
	    }
		gpio_direction_output(qyboard->dc5v0_en.pin, qyboard->dc5v0_en.active_level);
								gpio_export(qyboard->dc5v0_en.pin, 0);
		printk("*****dc5v0_en %d init ok, active_level = %d.\n",qyboard->dc5v0_en.pin,  qyboard->dc5v0_en.active_level);
	}

	
	/* 7v for amp enable */
	qyboard->dc7v0_en.pin = of_get_named_gpio_flags(np, "dc7v0-en", 0, &flags);
	if (qyboard->dc7v0_en.pin < 0) {
		printk("%s() Can not read property dc7v0_en.pin\n", __FUNCTION__);
		qyboard->dc7v0_en.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->dc7v0_en.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->dc7v0_en.pin, "dc7v0-en");
	    if (ret != 0) {
		    printk("%s request dc7v0_en error", __func__);
	    }
		gpio_direction_output(qyboard->dc7v0_en.pin, qyboard->dc7v0_en.active_level);
		gpio_export(qyboard->dc7v0_en.pin, 0);		
		printk("*****dc7v0-en %d init ok, active_level = %d.\n",qyboard->dc7v0_en.pin,  qyboard->dc7v0_en.active_level);
	}

	/* 12v enable */
	qyboard->dc12v0_en.pin = of_get_named_gpio_flags(np, "dc12v0-en", 0, &flags);
	if (qyboard->dc12v0_en.pin < 0) {
		printk("%s() Can not read property e31_pwren.pin\n", __FUNCTION__);
		qyboard->dc12v0_en.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->dc12v0_en.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->dc12v0_en.pin, "dc12v0-en");
	    if (ret != 0) {
		    printk("%s request dc12v0-en error", __func__);
	    }
		gpio_direction_output(qyboard->dc12v0_en.pin, qyboard->dc12v0_en.active_level);
		gpio_export(qyboard->dc12v0_en.pin, 0);		
		printk("*****dc12v0-en %d init ok, active_level = %d.\n",qyboard->dc12v0_en.pin,  qyboard->dc12v0_en.active_level);
	}	
	mdelay(10);
	
	/* hub reset */
	qyboard->hub_rst.pin = of_get_named_gpio_flags(np, "hub-rst", 0, &flags);
	if (qyboard->hub_rst.pin < 0) {
		printk("%s() Can not read property hub_rst.pin\n", __FUNCTION__);
		qyboard->hub_rst.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->hub_rst.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->hub_rst.pin, "hub-rst");
	    if (ret != 0) {
		    printk("%s request hub-rst error", __func__);
	    }
		gpio_direction_output(qyboard->hub_rst.pin, 0);		
		mdelay(50);
		gpio_direction_output(qyboard->hub_rst.pin, 1);		
		gpio_export(qyboard->hub_rst.pin, 0);

		printk("*****hub-rst  %d init ok, active_level = %d.\n",qyboard->hub_rst.pin, qyboard->hub_rst.active_level);
	}
	
	/* dvp power enable */
        qyboard->dvp_en.pin = of_get_named_gpio_flags(np, "dvp-en", 0, &flags);
        if (qyboard->dvp_en.pin < 0) {
                printk("%s() Can not read property dvp_en.pin\n", __FUNCTION__);
                qyboard->dvp_en.pin = INVALID_GPIO;
        }
        else
        {
            qyboard->dvp_en.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
            ret = gpio_request(qyboard->dvp_en.pin, "dvp-en");
            if (ret != 0) {
                    printk("%s request dvp-en error", __func__);
            }
	gpio_direction_output(qyboard->dvp_en.pin, qyboard->dvp_en.active_level);
        gpio_export(qyboard->dvp_en.pin, 0);
        printk("*****dvp-en %d init ok, active_level = %d.\n",qyboard->dvp_en.pin,  qyboard->dvp_en.active_level);
        }

	/* 4G init enable mode */
	qyboard->lte_flight.pin = of_get_named_gpio_flags(np, "lte-flight", 0, &flags);
	if (qyboard->lte_flight.pin < 0) {
		printk("%s() Can not read property lte_flight.pin\n", __FUNCTION__);
		qyboard->lte_flight.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->lte_flight.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->lte_flight.pin, "lte-flight");
	    if (ret != 0) {
		    printk("%s request lte-flight error", __func__);
	    }
		gpio_direction_output(qyboard->lte_flight.pin, qyboard->lte_flight.active_level);
		gpio_export(qyboard->lte_flight.pin, 0);		
		printk("*****lte-flight %d init ok, active_level = %d.\n",qyboard->lte_flight.pin,  qyboard->lte_flight.active_level);
	}
	
	/* 4G reset */
	qyboard->lte_rst.pin = of_get_named_gpio_flags(np, "lte-rst", 0, &flags);
	if (qyboard->lte_rst.pin < 0) {
		printk("%s() Can not read property lte_rst.pin\n", __FUNCTION__);
		qyboard->lte_rst.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->lte_rst.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->lte_rst.pin, "lte-rst");
	    if (ret != 0) {
		    printk("%s request lte_rst error", __func__);
	    }
		gpio_direction_output(qyboard->lte_rst.pin, 1);
		mdelay(200);
		gpio_direction_output(qyboard->lte_rst.pin, 0);
		gpio_export(qyboard->lte_rst.pin, 0);		
		printk("*****lte-rst %d init ok, active_level = %d.\n",qyboard->lte_rst.pin,  0);
	}

	/* buzzer init */
	qyboard->buzzer_ctrl.pin = of_get_named_gpio_flags(np, "buzzer-ctrl", 0, &flags);
	if (qyboard->buzzer_ctrl.pin < 0) {
		printk("%s() Can not read property buzzer_ctrl.pin\n", __FUNCTION__);
		qyboard->buzzer_ctrl.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->buzzer_ctrl.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->buzzer_ctrl.pin, "buzzer-ctrl");
	    if (ret != 0) {
		    printk("%s request buzzer_ctrl error", __func__);
	    }
		for(i = 0; i < 1000;i++)
		{
			if (i%2 == 0)
			{
				gpio_direction_output(qyboard->buzzer_ctrl.pin, 0);
			}
			else
			{
				gpio_direction_output(qyboard->buzzer_ctrl.pin, 1);
			}
			udelay(185);
		}
		gpio_export(qyboard->buzzer_ctrl.pin, 0);		
		printk("*****buzzer_ctrl %d init ok, active_level = %d.\n",qyboard->buzzer_ctrl.pin,  qyboard->buzzer_ctrl.active_level);
	}

		/* user led init */
	qyboard->user_led.pin = of_get_named_gpio_flags(np, "user-led", 0, &flags);
	if (qyboard->user_led.pin < 0) {
		printk("%s() Can not read property user_led.pin\n", __FUNCTION__);
		qyboard->user_led.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->user_led.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->user_led.pin, "user-led");
	    if (ret != 0) {
		    printk("%s request user_led error", __func__);
	    }
		gpio_direction_output(qyboard->user_led.pin, 1);
		gpio_export(qyboard->user_led.pin, 0);		
		printk("*****user_led %d init ok, active_level = %d.\n",qyboard->user_led.pin,  qyboard->user_led.active_level);
	}

	/* hardware version bit0  read from board */
	qyboard->hardware_ver0.pin = of_get_named_gpio_flags(np, "hardware-ver0", 0, &flags);
	if (qyboard->hardware_ver0.pin < 0) {
		printk("%s() Can not read property hardware_ver0.pin\n", __FUNCTION__);
		qyboard->hardware_ver0.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->hardware_ver0.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->hardware_ver0.pin, "hardware-ver0");
	    if (ret != 0) {
		    printk("%s request hardware-ver0 error", __func__);
	    }
		ver[1] = gpio_direction_input(qyboard->hardware_ver0.pin);
		gpio_export(qyboard->hardware_ver0.pin, 0);		
	}

	
	/* hardware version bit1  read from board */
	qyboard->hardware_ver1.pin = of_get_named_gpio_flags(np, "hardware-ver1", 0, &flags);
	if (qyboard->hardware_ver1.pin < 0) {
		printk("%s() Can not read property hardware-ver1.pin\n", __FUNCTION__);
		qyboard->hardware_ver1.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->hardware_ver1.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->hardware_ver1.pin, "hardware-ver1");
	    if (ret != 0) {
		    printk("%s request hardware-ver1 error", __func__);
	    }
		ver[1] = gpio_direction_input(qyboard->hardware_ver1.pin);
		gpio_export(qyboard->hardware_ver1.pin, 0);		
	}

	/* hardware version bit2  read from board */
	qyboard->hardware_ver2.pin = of_get_named_gpio_flags(np, "hardware-ver2", 0, &flags);
	if (qyboard->hardware_ver2.pin < 0) {
		printk("%s() Can not read property hardware-ver2.pin\n", __FUNCTION__);
		qyboard->hardware_ver2.pin = INVALID_GPIO;
	}
	else
	{
	    qyboard->hardware_ver2.active_level = (flags & OF_GPIO_ACTIVE_LOW)? 0:1;
	    ret = gpio_request(qyboard->hardware_ver2.pin, "hardware-ver2");
	    if (ret != 0) {
		    printk("%s request hardware_ver2 error", __func__);
	    }
		ver[2] = gpio_direction_input(qyboard->hardware_ver2.pin);
		gpio_export(qyboard->hardware_ver2.pin, 0);		
	}
	printk("################Board Hardware Version: 0x%x.0x%x.0x%x\n",ver[1], ver[1], ver[2]);

	if (0 != reserved_gpio_init(pdev)){
		printk("user gpio init failed.\n");
	}else{
		printk("user gpio init success.\n");
	};
	
	return 0;
}


static int gpio_init_remove(struct platform_device *pdev)
{
	struct qyboard_priv *qyboard = platform_get_drvdata(pdev);

	kfree(qyboard);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id of_qy_gpio_match[] = {
	{ .compatible = "xinx,board-gpio-init" },
	{ /* Sentinel */ }
};
#endif
static struct platform_driver gpio_init_driver = {
	.probe		= gpio_init_probe,
	.remove		= gpio_init_remove,
	.driver		= {
		.name	= "qy-gpio",
#ifdef CONFIG_OF
		.of_match_table	= of_qy_gpio_match,
#endif
		.owner	= THIS_MODULE,
	},
};

static int __init qyboard_gpio_init(void)
{
	int ret;

	printk("qyboard_gpio_init...\n");

	ret = platform_driver_register(&gpio_init_driver);
	if (ret != 0)
		platform_driver_unregister(&gpio_init_driver);
	
	return ret;
}

static void __exit qyboard_gpio_exit(void)
{
	platform_driver_unregister(&gpio_init_driver);
}

module_init(qyboard_gpio_init);
module_exit(qyboard_gpio_exit);

